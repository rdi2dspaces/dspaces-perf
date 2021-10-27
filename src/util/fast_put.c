#include <argp.h>
#include <inttypes.h>
#include <stdlib.h>

#include <dspaces.h>

#include <common.h>

#include <apex.h>

static struct argp_option options[] = {{"rankdims", 'r', "DIMS"},
                                       {"datadims", 'd', "DIMS"},
                                       {"steps", 's', "NUM"},
                                       {"putcount", 'p', "NUM"},
                                       {0}};

struct arguments {
    int ndim;
    int nput;
    uint64_t *rank_dims;
    uint64_t *data_dims;
    int steps;
    int ranks;
};

static error_t parse_opt(int key, char *arg, struct argp_state *state)
{
    struct arguments *arguments = state->input;
    int dims;
    int i, ranks = 1;

    switch(key) {
    case 'r':
    case 'd':
        if(key == 'r') {
            arguments->rank_dims = parse_cdd(arg, &dims);
            for(i = 0; i < dims; i++) {
                ranks *= arguments->rank_dims[i];
            }
            if(ranks != arguments->ranks) {
                fprintf(
                    stderr,
                    "rankdims should multiply to total number of MPI ranks.\n");
                argp_usage(state);
            }
        } else {
            arguments->data_dims = parse_cdd(arg, &dims);
        }
        if(arguments->ndim == 0) {
            arguments->ndim = dims;
        } else if(arguments->ndim > 0 && arguments->ndim != dims) {
            fprintf(
                stderr,
                "rankdims and datadims should have the same dimensionality.\n");
            argp_usage(state);
        }
        break;
    case 's':
        arguments->steps = atoi(arg);
        break;
    case 'p':
        arguments->nput = atoi(arg);
        break;
    default:
        return ARGP_ERR_UNKNOWN;
    }

    return (0);
}

static struct argp argp = {options, parse_opt, 0, 0};

void get_opts(int argc, char **argv, int rank, int comm_size,
              struct arguments *opts)
{
    int i;

    opts->ranks = comm_size;
    argp_parse(&argp, argc, argv, 0, 0, opts);

    if(opts->ndim == 0) {
        opts->ndim = 1;
    }
    if(!opts->rank_dims) {
        if(rank == 0) {
            printf("Using default rank geometry.\n");
        }
        opts->rank_dims = malloc(sizeof(*opts->rank_dims) * opts->ndim);
        opts->rank_dims[0] = comm_size;
        for(i = 1; i < opts->ndim; i++) {
            opts->rank_dims[i] = 1;
        }
    }
    if(!opts->data_dims) {
        if(rank == 0) {
            printf("Using default data geometry.\n");
        }
        opts->data_dims = malloc(sizeof(*opts->data_dims) * opts->ndim);
        for(i = 0; i < opts->ndim; i++) {
            opts->data_dims[i] = 1;
        }
    }

    if(!opts->steps) {
        opts->steps = 5;
    }

    if(!opts->nput) {
        opts->nput = 1;
    }
}

void print_opts(struct arguments *opts)
{
    int i;

    printf("fast_put configuration:\n"
           "-----------------------\n");

    printf("# Steps: %i\n", opts->steps);
    printf("Puts per rank per step: %i\n", opts->nput);
    printf("Rank geometry: %" PRIu64, opts->rank_dims[0]);
    for(i = 1; i < opts->ndim; i++) {
        printf("x%" PRIu64, opts->rank_dims[i]);
    }
    printf("\nPer-rank data domain: %" PRIu64, opts->data_dims[0]);
    for(i = 1; i < opts->ndim; i++) {
        printf("x%" PRIu64, opts->data_dims[i]);
    }
    printf("\n-----------------------\n");
}

double **init_buffers(struct arguments *opts)
{
    double **buffers;
    uint64_t num_elem = 1;
    int i, j;

    buffers = malloc(sizeof(*buffers) * opts->nput);
    for(i = 0; i < opts->ndim; i++) {
        num_elem *= opts->data_dims[i];
    }

    for(i = 0; i < opts->nput; i++) {
        buffers[i] = malloc(sizeof(*buffers[i]) * num_elem);
        for(j = 0; j < num_elem; j++) {
            buffers[i][j] = i + j;
        }
    }

    return (buffers);
}

char **init_vars(dspaces_client_t ds, struct arguments *opts)
{
    char **var_names;
    char *var_name_buf;
    uint64_t gdim[opts->ndim];
    int i;

    for(i = 0; i < opts->ndim; i++) {
        gdim[i] = opts->rank_dims[i] * opts->data_dims[i];
    }

    var_name_buf = malloc(sizeof(*var_name_buf) * 128 * opts->nput);
    var_names = malloc(sizeof(*var_names) * opts->nput);
    for(i = 0; i < opts->nput; i++) {
        var_names[i] = &var_name_buf[128 * i];
    }
    for(i = 0; i < opts->nput; i++) {
        sprintf(var_names[i], "put_var_%i", i);
        dspaces_define_gdim(ds, var_names[i], opts->ndim, gdim);
    }

    return(var_names);
}

void set_bounds(struct arguments *opts, int rank, uint64_t **lb, uint64_t **ub)
{
    int rank_pos;
    int i;

    *lb = malloc(sizeof(**lb) * opts->ndim);
    *ub = malloc(sizeof(**ub) * opts->ndim);

    for(i = 0; i < opts->ndim; i++) {
        rank_pos = rank % opts->rank_dims[i];
        rank /= opts->rank_dims[i];
        (*lb)[i] = rank_pos * opts->data_dims[i];
        (*ub)[i] = ((rank_pos + 1) * opts->data_dims[i]) - 1;
    }
}

int main(int argc, char **argv)
{
    struct arguments opts = {0};
    int rank, comm_size;
    MPI_Comm comm;
    dspaces_client_t ds;
    dspaces_put_req_t *reqs;
    double **buffers;
    uint64_t *ub, *lb;
    char **var_names;
    int num_iter;
    int i, j;

    MPI_Init(NULL, NULL);
    comm = MPI_COMM_WORLD;
    MPI_Comm_rank(comm, &rank);
    MPI_Comm_size(comm, &comm_size);

    get_opts(argc, argv, rank, comm_size, &opts);

    if(rank == 0) {
        print_opts(&opts);
    }

    reqs = malloc(sizeof(*reqs) * opts.nput);
    buffers = init_buffers(&opts);
    set_bounds(&opts, rank, &lb, &ub);

    apex_init("fast put", rank, comm_size);
    APEX_NAME_TIMER_START(1, "dspaces_init");
    dspaces_init_mpi(comm, &ds);
    APEX_TIMER_STOP(1);

    var_names = init_vars(ds, &opts);

    for(i = 0; i < opts.steps; i++) {
        MPI_Barrier(comm);
        APEX_NAME_TIMER_START(2, "do_iputs");
        for(j = 0; j < opts.nput; j++) {
            reqs[j] = dspaces_iput(ds, var_names[j], i, sizeof(**buffers),
                                   opts.ndim, lb, ub, buffers[j], 0, 0);
        }
        APEX_TIMER_STOP(2);

        APEX_NAME_TIMER_START(3, "put_wait");
        for(j = 0; j < opts.nput; j++) {
            dspaces_check_put(ds, reqs[j], 1); 
        }
        APEX_TIMER_STOP(3);
    }

    MPI_Barrier(comm);
    if(rank == 0) {
        dspaces_kill(ds);
    }

    dspaces_fini(ds);

    apex_finalize();

    MPI_Finalize();

    free(ub);
    free(lb);
    for(i = 0; i < opts.nput; i++) {
        free(buffers[i]);
    }
    free(buffers);
    free(reqs);
    free(var_names[0]);
    free(var_names);
    free(opts.rank_dims);
    free(opts.data_dims);

    return (0);
}
