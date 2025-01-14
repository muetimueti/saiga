﻿/**
 * Copyright (c) 2017 Darius Rückert
 * Licensed under the MIT License.
 * See LICENSE file for more information.
 */

#pragma once

#include "saiga/opengl/assets/all.h"
#include "saiga/opengl/assets/objAssetLoader.h"
#include "saiga/opengl/rendering/deferredRendering/deferredRendering.h"
#include "saiga/opengl/rendering/renderer.h"
#include "saiga/core/sdl/sdl_camera.h"
#include "saiga/core/sdl/sdl_eventhandler.h"
#include "saiga/opengl/window/sdl_window.h"
#include "saiga/opengl/world/proceduralSkybox.h"

#include "physics.h"
using namespace Saiga;


struct PhysicAssetObject : public SimpleAssetObject
{
    btRigidBody* rigidBody;

    void loadFromRigidbody()
    {
        btTransform trans;
        rigidBody->getMotionState()->getWorldTransform(trans);
        setPosition(toGLM(trans.getOrigin()));
        rot = toGLM(trans.getRotation());
    }
};

class Sample : public Updating, public DeferredRenderingInterface, public SDL_KeyListener
{
   public:
    SDLCamera<PerspectiveCamera> camera;

    BulletPhysics physics;
    std::vector<PhysicAssetObject> cubes;

    std::shared_ptr<Asset> cubeAsset;
    SimpleAssetObject groundPlane;

    ProceduralSkybox skybox;

    std::shared_ptr<DirectionalLight> sun;
    std::shared_ptr<Texture> t;

    Sample(Saiga::OpenGLWindow& window, Saiga::OpenGLRenderer& renderer);
    ~Sample();

    void initBullet();

    void update(float dt) override;
    void interpolate(float dt, float interpolation) override;
    void render(Camera* cam) override;
    void renderDepth(Camera* cam) override;
    void renderOverlay(Camera* cam) override;
    void renderFinal(Camera* cam) override;

    void keyPressed(SDL_Keysym key) override;
    void keyReleased(SDL_Keysym key) override;
};
