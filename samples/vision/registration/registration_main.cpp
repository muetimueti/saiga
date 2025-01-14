﻿/**
 * Copyright (c) 2017 Darius Rückert
 * Licensed under the MIT License.
 * See LICENSE file for more information.
 */
#include "saiga/vision/icp/ICPAlign.h"
#include "saiga/vision/slam/Trajectory.h"
#include "saiga/vision/util/Random.h"

using namespace Saiga;

class RegistrationTest
{
   public:
    RegistrationTest(int N)
    {
        // Get random ground truth
        groundTruthTransformation      = Random::randomSE3();
        groundTruthTransformationScale = sim3(groundTruthTransformation, gtscale);


        std::cout << groundTruthTransformation << std::endl;
        std::cout << groundTruthTransformationScale << std::endl;
        auto test = se3Scale(groundTruthTransformationScale);
        std::cout << test.first << " " << test.second << std::endl;
        std::cout << std::endl;

        std::cout << groundTruthTransformationScale.inverse() << std::endl;
        test = se3Scale(groundTruthTransformationScale.inverse());
        std::cout << test.first << " " << test.second << std::endl;


        std::cout << std::endl;
        Quat eigR = groundTruthTransformationScale.rxso3().quaternion().normalized();
        Vec3 eigt = groundTruthTransformationScale.translation();
        double s  = groundTruthTransformationScale.scale();
        eigt *= (1. / s);  //[R t/s;0 1]

        SE3 correctedTiw = SE3(eigR, eigt);

        std::cout << correctedTiw << std::endl;
        std::cout << correctedTiw.inverse() << std::endl;
        std::cout << se3Scale(groundTruthTransformationScale.inverse()).first.inverse() << std::endl;


        std::terminate();


        // create random point cloud
        // and transform by inverse ground truth transformation
        AlignedVector<Vec3> points, pointsTransformed, pointsTransformedScaled;
        auto invT  = groundTruthTransformation.inverse();
        auto invTs = groundTruthTransformationScale.inverse();
        for (int i = 0; i < N; ++i)
        {
            Vec3 p = Vec3::Random();
            points.push_back(p);
            pointsTransformed.push_back(invT * p);
            pointsTransformedScaled.push_back(invTs * p);
        }


        // create saiga icp corrs
        for (int i = 0; i < N; ++i)
        {
            ICP::Correspondence c;
            c.refPoint = points[i];

            c.srcPoint = pointsTransformed[i];
            pointCloud.push_back(c);

            c.srcPoint = pointsTransformedScaled[i];
            pointCloudScaled.push_back(c);
        }

        // create saiga trajectory
        for (int i = 0; i < N; ++i)
        {
            SE3 r           = Random::randomSE3();
            r.translation() = points[i];
            Sim3 rs         = sim3(r, 1);

            gt.emplace_back(i, r);
            tracking.emplace_back(i, invT * r);

            Sim3 transformedSe3 = invTs * rs;
            SE3 backToSe3       = se3Scale(transformedSe3).first;

            trackingScaled.emplace_back(i, backToSe3);
        }
    }

    void pointCloudSE3()
    {
        std::cout << "Testing SE3 registration..." << std::endl;
        auto result = ICP::pointToPointDirect(pointCloud);

        auto et = translationalError(groundTruthTransformation, result);
        auto rt = rotationalError(groundTruthTransformation, result);

        std::cout << "Error T/R: " << et << " " << rt << std::endl;
        std::cout << std::endl;
    }

    void pointCloudSim3()
    {
        std::cout << "Testing Sim3 registration..." << std::endl;
        double scale;
        auto result = ICP::pointToPointDirect(pointCloudScaled, &scale);

        auto [gtse3, gts] = se3Scale(groundTruthTransformationScale);

        auto et = translationalError(gtse3, result);
        auto rt = rotationalError(gtse3, result);
        auto es = std::abs(gts - scale);

        std::cout << "Error T/R/S: " << et << " " << rt << " " << es << std::endl;
        std::cout << std::endl;
    }

    void trajectorySE3()
    {
        std::cout << "Testing SE3 trajectory alignment..." << std::endl;
        Trajectory::align(gt, tracking, false);

        auto ate = Statistics(Trajectory::ate(gt, tracking)).max;
        auto rpe = Statistics(Trajectory::rpe(gt, tracking)).max;
        std::cout << "ATE/RPE " << ate << " " << rpe << std::endl;
        std::cout << std::endl;
    }

    void trajectorySim3()
    {
        std::cout << "Testing Sim3 trajectory alignment..." << std::endl;
        Trajectory::align(gt, trackingScaled, true);
        auto ate = Statistics(Trajectory::ate(gt, trackingScaled)).max;
        auto rpe = Statistics(Trajectory::rpe(gt, trackingScaled)).max;
        std::cout << "ATE/RPE " << ate << " " << rpe << std::endl;
        std::cout << std::endl;
    }


    double gtscale = 5;
    SE3 groundTruthTransformation;
    AlignedVector<ICP::Correspondence> pointCloud;

    Sim3 groundTruthTransformationScale;
    AlignedVector<ICP::Correspondence> pointCloudScaled;


    Trajectory::TrajectoryType gt, tracking, trackingScaled;
};

int main(int, char**)
{
    RegistrationTest test(500);
    test.pointCloudSE3();
    test.pointCloudSim3();
    test.trajectorySE3();
    test.trajectorySim3();
    std::cout << "Done." << std::endl;
    return 0;
}
