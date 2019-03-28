/* first_glpk.c */
#include <stdio.h>
#include <stdlib.h>
#include <glpk.h>
#include <assert.h>

#define EPS 0.000000001


static int counter = 0;

void formulate_lp(int n, unsigned int capacity, double* CM, double* CP, unsigned int* cap, unsigned int* tm, int* fixed_start_comm_time, double* start_comm_time, int* fixed_start_comp_time, double* start_comp_time)
{
    glp_prob *lp;
    int ia[1+1000];
    int ja[1+1000];

    double ar[1+1000];


    int iter1, iter2;
    double L = 0;

    for(iter1 = 1; iter1 <= n; iter1++)
        L += CM[iter1] + CP[iter1];

    lp = glp_create_prob();

    glp_set_prob_name(lp, "lp_formulation_for_communication_proble");
    glp_set_obj_dir(lp, GLP_MIN);

    int ncols = 4 * n * (n+1) + 1 + 1;
    glp_add_cols(lp, ncols);

    int icol = 0;

    //n s_i variables to denote the start time of communication
    for(iter1 = 1; iter1 <= n; iter1++)
    {
        ++icol;

        char name[100];
        sprintf(name, "s%d", iter1); 
        glp_set_col_name(lp, icol, name);

        if(fixed_start_comm_time[iter1])
            glp_set_col_bnds(lp, icol, GLP_FX, start_comm_time[iter1], 0.0); 
        else
            glp_set_col_bnds(lp, icol, GLP_LO, 0.0, 0.0); 

    }

    //n e_i variables to denote the end time of communication
    for(iter1 = 1; iter1 <= n; iter1++)
    {
        ++icol;

        char name[100];
        sprintf(name, "e%d", iter1); 
        glp_set_col_name(lp, icol, name);

        if(fixed_start_comm_time[iter1])
            glp_set_col_bnds(lp, icol, GLP_FX, start_comm_time[iter1] + CM[iter1], 0.0); 
        else
            glp_set_col_bnds(lp, icol, GLP_LO, 0.0, 0.0); 

    }

    //n s'_i variables to denote the start time of computation
    for(iter1 = 1; iter1 <= n; iter1++)
    {
        ++icol;

        char name[100];
        sprintf(name, "s'%d", iter1); 
        glp_set_col_name(lp, icol, name);

        if(fixed_start_comp_time[iter1])
            glp_set_col_bnds(lp, icol, GLP_FX, start_comp_time[iter1], 0.0); 
        else
            glp_set_col_bnds(lp, icol, GLP_LO, 0.0, 0.0); 

    }

    //n e'_i variables to denote the end time of computation
    for(iter1 = 1; iter1 <= n; iter1++)
    {
        ++icol;

        char name[100];
        sprintf(name, "e'%d", iter1); 
        glp_set_col_name(lp, icol, name);

        if(fixed_start_comp_time[iter1])
            glp_set_col_bnds(lp, icol, GLP_FX, start_comp_time[iter1] + CP[iter1], 0.0); 
        else
            glp_set_col_bnds(lp, icol, GLP_LO, 0.0, 0.0); 

    }

    //n*n binary variables to denote order of communication
    //a_i_j = 1 => e_j <= s_i
    for(iter1 = 1; iter1 <= n; iter1++)
    {
        for(iter2 = 1; iter2 <= n; iter2++)
        {
            ++icol;

            char name[100];
            sprintf(name, "a_%d_%d", iter1, iter2); 
            glp_set_col_name(lp, icol, name);
            glp_set_col_kind(lp, icol, GLP_BV);
            
            if(iter1 == iter2)
                glp_set_col_bnds(lp, icol, GLP_FX, 0.0, 0.0); 
            //else
            //    glp_set_col_bnds(lp, icol, GLP_LO, 0.0, 0.0); 

            //glp_set_col_kind(lp, icol, GLP_BV);

            
        }
    }


    //n*n binary variables to denote order of computation
    //b_i_j = 1 => e'_j <= s'_i
    for(iter1 = 1; iter1 <= n; iter1++)
    {
        for(iter2 = 1; iter2 <= n; iter2++)
        {
            ++icol;

            char name[100];
            sprintf(name, "b_%d_%d", iter1, iter2); 
            glp_set_col_name(lp, icol, name);
            glp_set_col_kind(lp, icol, GLP_BV);
            if(iter1 == iter2)
                glp_set_col_bnds(lp, icol, GLP_FX, 0.0, 0.0); 

            
        }
    }

    //n*n binary variables to denote communication order with other computations
    //c_i_j = 1 => e'_j <= s_i
    for(iter1 = 1; iter1 <= n; iter1++)
    {
        for(iter2 = 1; iter2 <= n; iter2++)
        {
            ++icol;

            char name[100];
            sprintf(name, "c_%d_%d", iter1, iter2); 
            glp_set_col_name(lp, icol, name);
            glp_set_col_kind(lp, icol, GLP_BV);
            if(iter1 == iter2)
                glp_set_col_bnds(lp, icol, GLP_FX, 0.0, 0.0); 

            
        }
    }


    //n*n binary variables to denote computation order with other communications
    //d_i_j = 1 => s_j <= e'_i
    for(iter1 = 1; iter1 <= n; iter1++)
    {
        for(iter2 = 1; iter2 <= n; iter2++)
        {
            ++icol;

            char name[100];
            sprintf(name, "d_%d_%d", iter1, iter2); 
            glp_set_col_name(lp, icol, name);
            glp_set_col_kind(lp, icol, GLP_BV);
            if(iter1 == iter2)
                glp_set_col_bnds(lp, icol, GLP_FX, 0.0, 0.0); 

            
        }
    }

    // objective variable to minimize
    {
        ++icol;
        char name[100];
        sprintf(name, "tmax"); 
        glp_set_col_name(lp, icol, name);
        glp_set_col_bnds(lp, icol, GLP_LO, 0.0, 0.0); 

        glp_set_obj_coef(lp, icol, 1.0);

    }
    // set communication time  to minimize
    {
        ++icol;
        char name[100];
        sprintf(name, "tmax_comm"); 
        glp_set_col_name(lp, icol, name);
        glp_set_col_bnds(lp, icol, GLP_LO, 0.0, 0.0); 

        glp_set_obj_coef(lp, icol, 0.0);

    }


    assert(icol == ncols); 

    int nrows = n * (8*n - 2) + n;
    
    //a_i_j >= c_i_j  and d_i_j >= b_i_j
    // a/b_i_j + a/b_j_i = 1
    nrows += 2*n*n + n*(n-1);

    glp_add_rows(lp, nrows);
    int irow = 0;

    int ncoeffs = 0;
    for(iter1 = 1; iter1 <= n; iter1++)
    {
        //all constraints for ith task
        char name[100];

        //e'_i <= tmax
        irow++;
        //char name[100];
        sprintf(name, "e'_%d<=tmax", iter1); 
        glp_set_row_name(lp, irow, name);
        glp_set_row_bnds(lp, irow, GLP_UP, 0.0, 0.0);

        ncoeffs++; ia[ncoeffs] = irow; ja[ncoeffs] = 3*n + iter1; ar[ncoeffs] = 1;
        printf("ja[%d] = %d\n", ncoeffs, ja[ncoeffs]);
        ncoeffs++; ia[ncoeffs] = irow; ja[ncoeffs] = 4 * n * (n+1) + 1 ; ar[ncoeffs] = -1;

        //e_i <= tmax_comm
        irow++;
        //char name[100];
        sprintf(name, "e_%d<=tmax_comm", iter1); 
        glp_set_row_name(lp, irow, name);
        glp_set_row_bnds(lp, irow, GLP_UP, 0.0, 0.0);

        ncoeffs++; ia[ncoeffs] = irow; ja[ncoeffs] = n + iter1; ar[ncoeffs] = 1;
        printf("ja[%d] = %d\n", ncoeffs, ja[ncoeffs]);
        ncoeffs++; ia[ncoeffs] = irow; ja[ncoeffs] = 4 * n * (n+1) + 2 ; ar[ncoeffs] = -1;


        //s_i + CM(i) = e_i
        irow++;
        //char name[100];
        sprintf(name, "s_%d+comm_time=e_%d", iter1, iter1); 
        glp_set_row_name(lp, irow, name);
        glp_set_row_bnds(lp, irow, GLP_FX, CM[iter1], 0.0);

        ncoeffs++; ia[ncoeffs] = irow; ja[ncoeffs] = iter1; ar[ncoeffs] = -1;
        ncoeffs++; ia[ncoeffs] = irow; ja[ncoeffs] = n + iter1; ar[ncoeffs] = 1;

        //s'_i + CP(i) = e'_i
        irow++;
        //char name[100];
        sprintf(name, "s'_%d+comp_time=e'_%d", iter1, iter1); 
        glp_set_row_name(lp, irow, name);
        glp_set_row_bnds(lp, irow, GLP_FX, CP[iter1], 0.0);

        ncoeffs++; ia[ncoeffs] = irow; ja[ncoeffs] = 2*n + iter1; ar[ncoeffs] = -1;
        ncoeffs++; ia[ncoeffs] = irow; ja[ncoeffs] = 3*n + iter1; ar[ncoeffs] = 1;

        //e_i <= s'_i
        irow++;
        //char name[100];
        sprintf(name, "e_%d<=s'_%d", iter1, iter1); 
        glp_set_row_name(lp, irow, name);
        glp_set_row_bnds(lp, irow, GLP_UP, 0.0, 0.0);

        ncoeffs++; ia[ncoeffs] = irow; ja[ncoeffs] = n + iter1; ar[ncoeffs] = 1;
        ncoeffs++; ia[ncoeffs] = irow; ja[ncoeffs] = 2*n + iter1; ar[ncoeffs] = -1;



        //exclusiveness on communication channel
        for(iter2 = 1; iter2 <= n; iter2++)
        {
            if(iter1 == iter2) continue;

            // \forall j notequal to i, e_j <= s_i + (1-a_i_j)L
            irow++;
            //char name[100];
            sprintf(name, "e_%d<=s_%d+(1-a_%d_%d)L", iter2, iter1, iter1, iter2);
            glp_set_row_name(lp, irow, name);
            glp_set_row_bnds(lp, irow, GLP_UP, 0.0, L);

            ncoeffs++; ia[ncoeffs] = irow; ja[ncoeffs] = iter1; ar[ncoeffs] = -1;
            ncoeffs++; ia[ncoeffs] = irow; ja[ncoeffs] = n + iter2; ar[ncoeffs] = 1;
            ncoeffs++; ia[ncoeffs] = irow; ja[ncoeffs] = 4*n + n*(iter1-1) + iter2; ar[ncoeffs] = L;


            // \forall j notequal to i, e_i <= s_j + a_i_j L
            irow++;
            //char name[100];
            sprintf(name, "e_%d<=s_%d+a_%d_%dL", iter1, iter2, iter1, iter2);
            glp_set_row_name(lp, irow, name);
            glp_set_row_bnds(lp, irow, GLP_UP, 0.0, 0.0);

            ncoeffs++; ia[ncoeffs] = irow; ja[ncoeffs] = iter2; ar[ncoeffs] = -1;
            ncoeffs++; ia[ncoeffs] = irow; ja[ncoeffs] = n + iter1; ar[ncoeffs] = 1;
            ncoeffs++; ia[ncoeffs] = irow; ja[ncoeffs] = 4*n + n*(iter1-1) + iter2; ar[ncoeffs] = -L;
        }

 

        //exclusiveness on computation channel
        for(iter2 = 1; iter2 <= n; iter2++)
        {
            if(iter1 == iter2) continue;

            // \forall j notequal to i, e'_j <= s'_i + (1-b_i_j)L
            irow++;
            //char name[100];
            sprintf(name, "e'_%d<=s'_%d+(1-b_%d_%d)L", iter2, iter1, iter1, iter2);
            glp_set_row_name(lp, irow, name);
            glp_set_row_bnds(lp, irow, GLP_UP, 0.0, L);

            ncoeffs++; ia[ncoeffs] = irow; ja[ncoeffs] = 2*n + iter1; ar[ncoeffs] = -1;
            ncoeffs++; ia[ncoeffs] = irow; ja[ncoeffs] = 3*n + iter2; ar[ncoeffs] = 1;
            ncoeffs++; ia[ncoeffs] = irow; ja[ncoeffs] = n*n + 4*n + n*(iter1-1) + iter2; ar[ncoeffs] = L;


            // \forall j notequal to i, e'_i <= s'_j + b_i_j L
            irow++;
            //char name[100];
            sprintf(name, "e'_%d<=s'_%d+b_%d_%dL", iter1, iter2, iter1, iter2);
            glp_set_row_name(lp, irow, name);
            glp_set_row_bnds(lp, irow, GLP_UP, 0.0, 0.0);

            ncoeffs++; ia[ncoeffs] = irow; ja[ncoeffs] = 2*n + iter2; ar[ncoeffs] = -1;
            ncoeffs++; ia[ncoeffs] = irow; ja[ncoeffs] = 3*n + iter1; ar[ncoeffs] = 1;
            ncoeffs++; ia[ncoeffs] = irow; ja[ncoeffs] = n*n + 4*n + n*(iter1-1) + iter2; ar[ncoeffs] = -L;
        }

        //order of computations with respect to ith communication
        for(iter2 = 1; iter2 <= n; iter2++)
        {
            if(iter1 == iter2) continue;

            // \forall j notequal to i, e'_j <= s_i + (1-c_i_j)L
            irow++;
            //char name[100];
            sprintf(name, "e'_%d<=s_%d+(1-c_%d_%d)L", iter2, iter1, iter1, iter2);
            glp_set_row_name(lp, irow, name);
            glp_set_row_bnds(lp, irow, GLP_UP, 0.0, L);

            ncoeffs++; ia[ncoeffs] = irow; ja[ncoeffs] = iter1; ar[ncoeffs] = -1;
            ncoeffs++; ia[ncoeffs] = irow; ja[ncoeffs] = 3*n + iter2; ar[ncoeffs] = 1;
            ncoeffs++; ia[ncoeffs] = irow; ja[ncoeffs] = 2*n*n + 4*n + n*(iter1-1) + iter2; ar[ncoeffs] = L;


            // \forall j notequal to i, s_i < e'_j + c_i_j L
            irow++;
            //char name[100];
            sprintf(name, "s_%d<=e'_%d+c_%d_%dL", iter1, iter2, iter1, iter2);
            glp_set_row_name(lp, irow, name);
            glp_set_row_bnds(lp, irow, GLP_UP, 0.0, -EPS);

            ncoeffs++; ia[ncoeffs] = irow; ja[ncoeffs] = iter1; ar[ncoeffs] = 1;
            ncoeffs++; ia[ncoeffs] = irow; ja[ncoeffs] = 3*n + iter2; ar[ncoeffs] = -1;
            ncoeffs++; ia[ncoeffs] = irow; ja[ncoeffs] = 2*n*n + 4*n + n*(iter1-1) + iter2; ar[ncoeffs] = -L;
        
        }

        //capacity constraint on communcation channel
        {
            //\sum_j, j != i, (a_i_r - c_i_r) CAP(r) + CAP(i) <= CAPACITY
            irow++;
            //char name[100];
            sprintf(name, "capacity_constraint(%d)_at_comm", iter1);
            glp_set_row_name(lp, irow, name);
            glp_set_row_bnds(lp, irow, GLP_UP, 0.0, capacity - cap[iter1]);

            for(iter2 = 1; iter2 <= n; iter2++)
            {
                if(iter1 == iter2) continue;

                int local_cap = cap[iter2];
                ncoeffs++; ia[ncoeffs] = irow; ja[ncoeffs] = 4*n + n*(iter1-1) + iter2; ar[ncoeffs] = local_cap;
                ncoeffs++; ia[ncoeffs] = irow; ja[ncoeffs] = 2*n*n + 4*n + n*(iter1-1) + iter2; ar[ncoeffs] = -local_cap;

            }
        }
        

        //order of communications with respect to ith computation
        for(iter2 = 1; iter2 <= n; iter2++)
        {
            if(iter1 == iter2) continue;

            // \forall j notequal to i, s_j < e'_i + (1-d_i_j)L
            irow++;
            //char name[100];
            sprintf(name, "s_%d<=e'_%d+(1-d_%d_%d)L", iter2, iter1, iter1, iter2);
            glp_set_row_name(lp, irow, name);
            glp_set_row_bnds(lp, irow, GLP_UP, 0.0, L-EPS);

            ncoeffs++; ia[ncoeffs] = irow; ja[ncoeffs] = iter2; ar[ncoeffs] = 1;
            ncoeffs++; ia[ncoeffs] = irow; ja[ncoeffs] = 3*n + iter1; ar[ncoeffs] = -1;
            ncoeffs++; ia[ncoeffs] = irow; ja[ncoeffs] = 3*n*n + 4*n + n*(iter1-1) + iter2; ar[ncoeffs] = L;


            // \forall j notequal to i, e'_i <= s_j + d_i_j L
            irow++;
            //char name[100];
            sprintf(name, "e'_%d<=s_%d+d_%d_%dL", iter1, iter2, iter1, iter2);
            glp_set_row_name(lp, irow, name);
            glp_set_row_bnds(lp, irow, GLP_UP, 0.0, 0.0);

            ncoeffs++; ia[ncoeffs] = irow; ja[ncoeffs] = iter2; ar[ncoeffs] = -1;
            ncoeffs++; ia[ncoeffs] = irow; ja[ncoeffs] = 3*n + iter1; ar[ncoeffs] = 1;
            ncoeffs++; ia[ncoeffs] = irow; ja[ncoeffs] = 3*n*n + 4*n + n*(iter1-1) + iter2; ar[ncoeffs] = -L;
        
        }

        //capacity constraint on computation channel due to temporary memory
        {
            //\sum_j, j != i, (d_i_r - b_i_r) CAP(r) + TM(i) <= CAPACITY
            irow++;
            //char name[100];
            sprintf(name, "capacity_constraint(%d)_at_comp", iter1);
            glp_set_row_name(lp, irow, name);
            glp_set_row_bnds(lp, irow, GLP_UP, 0.0, capacity - tm[iter1]);

            for(iter2 = 1; iter2 <= n; iter2++)
            {
                if(iter1 == iter2) continue;

                int local_cap = cap[iter2];
                ncoeffs++; ia[ncoeffs] = irow; ja[ncoeffs] = n*n + 4*n + n*(iter1-1) + iter2; ar[ncoeffs] = -local_cap;
                ncoeffs++; ia[ncoeffs] = irow; ja[ncoeffs] = 3*n*n + 4*n + n*(iter1-1) + iter2; ar[ncoeffs] = local_cap;

            }
        }
 
    }


    // a_i_j >= c_i_j
    // d_i_j >= b_i_j

    for(iter1 = 1; iter1 <= n; iter1++)
    {
        for(iter2 = 1; iter2 <= n; iter2++)
        {
            char name[100];

            irow++;
            sprintf(name, "a_%d_%d>=c_%d_%d", iter1, iter2, iter1, iter2);
            glp_set_row_name(lp, irow, name);
            glp_set_row_bnds(lp, irow, GLP_UP, 0.0, 0.0);

            ncoeffs++; ia[ncoeffs] = irow; ja[ncoeffs] = 4*n + n*(iter1-1) + iter2; ar[ncoeffs] = -1;
            ncoeffs++; ia[ncoeffs] = irow; ja[ncoeffs] = 2*n*n + 4*n + n*(iter1-1) + iter2; ar[ncoeffs] = 1;


            irow++;
            sprintf(name, "d_%d_%d>=b_%d_%d", iter1, iter2, iter1, iter2);
            glp_set_row_name(lp, irow, name);
            glp_set_row_bnds(lp, irow, GLP_UP, 0.0, 0.0);

            ncoeffs++; ia[ncoeffs] = irow; ja[ncoeffs] = 3*n*n + 4*n + n*(iter1-1) + iter2; ar[ncoeffs] = -1;
            ncoeffs++; ia[ncoeffs] = irow; ja[ncoeffs] = n*n + 4*n + n*(iter1-1) + iter2; ar[ncoeffs] = 1;

        }
    }

    //a/b_i_j + a/b_j_i = 1
    for(iter1 = 1; iter1 <= n; iter1++)
    {
        for(iter2 = iter1 + 1; iter2 <=n; iter2++)
        {
            char name[100];


            irow++;
            sprintf(name, "a_%d_%d+a_%d_%d=1", iter1, iter2, iter2, iter1);
            glp_set_row_bnds(lp, irow, GLP_FX, 1.0, 1.0);
            ncoeffs++; ia[ncoeffs] = irow; ja[ncoeffs] = 4*n + n*(iter1-1) + iter2; ar[ncoeffs] = 1;
            ncoeffs++; ia[ncoeffs] = irow; ja[ncoeffs] = 4*n + n*(iter2-1) + iter1; ar[ncoeffs] = 1;

            irow++;
            sprintf(name, "b_%d_%d+b_%d_%d=1", iter1, iter2, iter2, iter1);
            glp_set_row_bnds(lp, irow, GLP_FX, 1.0, 1.0);
            ncoeffs++; ia[ncoeffs] = irow; ja[ncoeffs] = n*n + 4*n + n*(iter1-1) + iter2; ar[ncoeffs] = 1;
            ncoeffs++; ia[ncoeffs] = irow; ja[ncoeffs] = n*n + 4*n + n*(iter2-1) + iter1; ar[ncoeffs] = 1;

    /*
            irow++;
            sprintf(name, "c_%d_%d+c_%d_%d=1", iter1, iter2, iter2, iter1);
            glp_set_row_bnds(lp, irow, GLP_FX, 1.0, 1.0);
            ncoeffs++; ia[ncoeffs] = irow; ja[ncoeffs] = 2*n*n + 4*n + n*(iter1-1) + iter2; ar[ncoeffs] = 1;
            ncoeffs++; ia[ncoeffs] = irow; ja[ncoeffs] = 2*n*n + 4*n + n*(iter2-1) + iter1; ar[ncoeffs] = 1;
            irow++;
            sprintf(name, "d_%d_%d+d_%d_%d=1", iter1, iter2, iter2, iter1);
            glp_set_row_bnds(lp, irow, GLP_FX, 1.0, 1.0);
            ncoeffs++; ia[ncoeffs] = irow; ja[ncoeffs] = 3*n*n + 4*n + n*(iter1-1) + iter2; ar[ncoeffs] = 1;
            ncoeffs++; ia[ncoeffs] = irow; ja[ncoeffs] = 3*n*n + 4*n + n*(iter2-1) + iter1; ar[ncoeffs] = 1;
    */
        }
    }



    assert(irow == nrows);



    glp_load_matrix(lp, ncoeffs, ia, ja, ar);
    char file_name[100];
    sprintf(file_name, "communication_data_transfer_%d.lp", counter++);
    //glp_write_lp(lp, NULL, "communication_data_transfer.lp");
//    glp_write_lp(lp, NULL, file_name);
    //glp_simplex(lp, NULL);

    glp_iocp parm;
    glp_init_iocp(&parm);
    parm.presolve = GLP_ON;

    int err = glp_intopt(lp, &parm);
    //int err = glp_intopt(lp, NULL);

    if(glp_mip_status(lp) == GLP_OPT)
    {
        glp_set_col_bnds(lp, ncols-1, GLP_FX, glp_mip_obj_val(lp), 0.0);
        glp_set_obj_coef(lp, ncols-1, 0.0);
        glp_set_obj_coef(lp, ncols, 1.0);

        sprintf(file_name, "communication_data_transfer_%d.lp", counter++);
//        glp_write_lp(lp, NULL, file_name);

        //glp_simplex(lp, NULL);
        int err = glp_intopt(lp, &parm);
        if(glp_mip_status(lp) == GLP_OPT)
        {
            /*
            for(iter1 = 1; iter1 <= ncols; iter1++)
            {
                printf("%s - %lf\n", glp_get_col_name(lp, iter1), glp_mip_col_val(lp, iter1));
            }
            */
        }
    }
    //    glp_write_lp(lp, NULL, "first1.lp");

    for(iter1 = 1; iter1 <= n; iter1++)
    {
        //start_comm_time[iter1] = glp_get_col_prim(lp, iter1);
        //start_comp_time[iter1] = glp_get_col_prim(lp, 2*n + iter1);
        start_comm_time[iter1] = glp_mip_col_val(lp, iter1);
        start_comp_time[iter1] = glp_mip_col_val(lp, 2*n + iter1);
    }


    glp_delete_prob(lp);
 
}

