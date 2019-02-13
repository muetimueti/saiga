/**
 * Copyright (c) 2017 Darius Rückert
 * Licensed under the MIT License.
 * See LICENSE file for more information.
 */
#include "mkl_test.h"

namespace Saiga
{
MKL_Test::MKL_Test()
{
    A.resize(n, m);
    B.resize(n, m);
    C.resize(n, m);
    x.resize(n);
    y.resize(n);

    // ============= Create the Eigen Recursive Data structures =============


    Saiga::Random::setSeed(357609435);
#if 1
    // fast creation for non symmetric matrix
    A.reserve(n * nnzr);
    for (int i = 0; i < n; ++i)
    {
        auto indices = Random::uniqueIndices(nnzr, m);
        std::sort(indices.begin(), indices.end());

        A.startVec(i);
        for (auto j : indices)
        {
            A.insertBackByOuterInner(i, j) = Block::Random();
        }
        x(i) = Vector::Random();
        y(i) = Vector::Random();
    }
    A.finalize();
#else
    // create a symmetric matrix
    std::vector<Eigen::Triplet<Block> > trips;
    trips.reserve(nnzr * n * 2);

    for (int i = 0; i < n; ++i)
    {
        auto indices = Random::uniqueIndices(nnzr, m);
        std::sort(indices.begin(), indices.end());

        for (auto j : indices)
        {
            if (i != j)
            {
                Block b = Block::Random();
                trips.emplace_back(i, j, b);
                trips.emplace_back(j, i, b.transpose());
            }
        }

        // Make sure we have a symmetric diagonal block
        Vector dv = Vector::Random();
        Block D   = dv * dv.transpose();

        // Make sure the matrix is positiv
        D.diagonal() += Vector::Ones() * 5;
        trips.emplace_back(i, i, D);

        x(i) = Vector::Random();
        y(i) = Vector::Random();
    }
    A.setFromTriplets(trips.begin(), trips.end());
    A.makeCompressed();

//        cout << expand(A) << endl;
#endif
    B = A;


    {
        // mkl matrix
        createBlockMKLFromEigen(A, &mkl_A, row_start, row_end, col_index, values, n, m, block_size);
        ex_x = expand(x);
        ex_y = expand(y);
        auto ret =
            createMKL(&mkl_B, row_start.data(), row_end.data(), col_index.data(), values.data(), n, m, block_size);
        mkl_A_desc.type = SPARSE_MATRIX_TYPE_GENERAL;
        mkl_B_desc      = mkl_A_desc;
        SAIGA_ASSERT(ret == SPARSE_STATUS_SUCCESS);
        mkl_set_num_threads_local(1);
        mkl_set_num_threads(1);
    }

#if 0
        {
            nist_A = BLAS_duscr_block_begin(n, m, block_size, block_size);
            cout << nist_A << endl;

            for (auto trip : trips)
            {
                auto ret = BLAS_duscr_insert_block(nist_A, trip.value().data(), block_size, 1, trip.row(), trip.col());
                SAIGA_ASSERT(ret == 0);
            }
        }
#endif

    cout << "Init done." << endl;


    // Print some stats
    cout << "." << endl;
    cout << "Block Size : " << block_size << "x" << block_size << endl;
    cout << "Matrix Size (in Blocks): " << n << "x" << m << endl;
    cout << "Matrix Size Total: " << n * block_size << "x" << m * block_size << endl;
    cout << "Non Zero blocks per row: " << nnzr << endl;
    cout << "Non Zero BLocks: " << nnzr * n << endl;
    cout << "Non Zeros: " << nnzr * n * block_size * block_size << endl;
    cout << "." << endl;
    cout << endl;
}

}  // namespace Saiga


int main(int argc, char* argv[])
{
    (void)argc;
    (void)argv;
    Saiga::EigenHelper::checkEigenCompabitilty<2765>();

    Saiga::MKL_Test t;
    if (vec_mult) t.sparseMatrixVector();
    if (mat_mult) t.sparseMatrixMatrix();
    if (cg_mult) t.sparseCG();
    return 0;
}