﻿/**
 * Copyright (c) 2017 Darius Rückert
 * Licensed under the MIT License.
 * See LICENSE file for more information.
 */

#include "saiga/camera/camera.h"
#include "saiga/framework/framework.h"
#include "saiga/geometry/AccelerationStructure.h"
#include "saiga/geometry/intersection.h"
#include "saiga/geometry/ray.h"
#include "saiga/image/image.h"
#include "saiga/model/objModelLoader.h"
#include "saiga/time/timer.h"

using namespace Saiga;

int main(int argc, char* args[])
{
    Saiga::SaigaParameters saigaParameters;
    Saiga::initSample(saigaParameters);
    Saiga::initSaiga(saigaParameters);

    int w = 500;
    int h = 500;

    PerspectiveCamera camera;
    camera.setProj(60.0f, 1, 0.1f, 50.0f, true);
    camera.setView(vec3(0, 3, 6), vec3(0, 0, 0), vec3(0, 1, 0));

    ObjModelLoader loader("teapot.obj");

    TriangleMesh<VertexNC, uint32_t> mesh;
    loader.toTriangleMesh(mesh);

    std::vector<Triangle> triangles;
    mesh.toTriangleList(triangles);


    AccelerationStructure::ObjectMedianBVH bf(triangles);

    cout << "Num triangles = " << triangles.size() << endl;


    TemplatedImage<ucvec3> img(w, h);

    {
        SAIGA_BLOCK_TIMER();
#pragma omp parallel for
        for (int i = 0; i < h; ++i)
        {
            for (int j = 0; j < w; ++j)
            {
                img(i, j) = ucvec3(255, 0, 0);

                vec3 dir = camera.inverseprojectToWorldSpace(vec2(j, i), 1, w, h);
                Ray ray(normalize(dir), camera.getPosition());

                auto inter = bf.getClosest(ray);
                if (inter && !inter.backFace)
                {
                    img(i, j) = ucvec3(0, 255, 0);
                }
            }
        }
    }
    img.save("raytracing.png");
}