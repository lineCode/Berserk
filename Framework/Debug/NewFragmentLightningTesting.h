//
// Created by Egor Orachyov on 29.07.2018.
//

#ifndef BERSERKENGINE_NEWFRAGMENTLIGHTNINGTESTING_H
#define BERSERKENGINE_NEWFRAGMENTLIGHTNINGTESTING_H

#include "Context/ApplicationContext.h"
#include "Managers/SceneManager.h"

#include "Strings/UtilityString.h"
#include "Shapes/Sphere.h"
#include "Shapes/Torus.h"

#include "Objects/Materials/BaseMaterial.h"
#include "Objects/Lights/SpotLight.h"
#include "Objects/Lights/PointLight.h"
#include "Objects/Lights/AmbientLight.h"
#include "Objects/Lights/DirectionalLight.h"

using namespace Berserk;

class RenderNodeActor : public Actor
{
public:

    RenderNodeActor(const CStaticString& name, FLOAT32 lifeTime = 0) :
            Actor(name, lifeTime),
            pointLight(CNAME("PointLight1"))
    {
        Material* material = gRenderSystem->getMaterialManagerPtr()->createMaterial(CNAME("Mat1"));
        RenderMesh* mesh = gRenderSystem->getRenderMeshManagerPtr()->createRenderMesh(CNAME("Mesh1"));

        material->setType(MaterialType::MT_BASIC);
        material->setAmbientComponent(Vector3f(0.1,0.1,0.1));
        material->setDiffuseComponent(Vector3f(0.3,0.7,0.2));
        material->setSpecularComponent(Vector3f(0.9,0.9,0.9));
        material->setShininess(12);

        UINT32   vertex_count = 3;
        VertexPN v[3];

        v[0].mPosition = Vector3f(0,1,0);
        v[0].mNormal = Vector3f(0,0,1);
        v[1].mPosition = Vector3f(-1,0,0);
        v[1].mNormal = Vector3f(0,0,1);
        v[2].mPosition = Vector3f(1,0,0);
        v[2].mNormal = Vector3f(0,0,1);

        UINT32  index_count = 3;
        UINT16  i[3] = {0,1,2};

        mesh->setType(MeshType::MT_PN);
        mesh->addGeometryInfo(v, vertex_count);
        mesh->addGeometryInfo(i, index_count);

        renderNode = gRenderSystem->createRenderNode();
        renderNode->setRenderNodeType(RenderNodeType::RNT_OBJECT);
        renderNode->setRenderMesh(mesh);
        renderNode->setMaterial(material);
        renderNode->setVisible(true);

        pointLight.setPosition(Vector3f(3,3,3));
        pointLight.setRadius(10);
        pointLight.setConstantAttenuation(1);
        pointLight.setLinearAttenuation(0.08);
        pointLight.setQuadraticAttenuation(0.005);
        pointLight.setLightIntensity(Vector3f(0,0.1,0));

        gRenderSystem->registerLightSource(&pointLight);
    }


    virtual ~RenderNodeActor()
    {
        gRenderSystem->deleteRenderNode(renderNode);
    }

    void onUpdate(FLOAT64 delta) override
    {
        renderNode->setTransformation(rotate(Vector3f(0.5, 0.1, 0.63), 0.011) * renderNode->getTransformation());
    }

private:

    RenderNode* renderNode;
    PointLight  pointLight;

};

class TorusActor : public Actor
{
public:

    TorusActor(const CStaticString &name, FLOAT32 lifeTime = 0) :
            Actor(name, lifeTime),
            material(CNAME("BlueOcean"))
    {
        mBuffer.init();

        Torus torus;
        torus.create(1.6, 0.4, 20, 20);
        torus.fill(mBuffer);
        torus.destroy();

        material.setAmbientColor(Vector3f(0.32, 0.102, 0.86));
        material.setDiffuseColor(Vector3f(0.33, 0.11, 0.87));
        material.setSpecularColor(Vector3f(0.35, 0.13, 0.91));
        material.setShininess(18);
    }

    virtual ~TorusActor()
    {
        mBuffer.destroy();
    }

    void onUpdate(FLOAT64 delta) override
    {
        addRotation(Vector3f(0.5,0.8,0.48), 0.012);

        gSceneManager->getRenderManager().queueTransformation(&mTransformation);
        gSceneManager->getRenderManager().queueBuffer((GPUBuffer*)&mBuffer);
        material.use();
    }

private:

    GLGPUBuffer mBuffer;
    BaseMaterial material;

};

class RenderActor : public Actor
{
public:

    RenderActor(const CStaticString &name, FLOAT32 lifeTime = 0) :
            Actor(name, lifeTime),
            material(CNAME("RedMaterial"))
    {
        addMovement(Vector3f(1,0,0));
    }

    virtual ~RenderActor()
    {

    }

    void onInit() override
    {
        mBuffer1 = new GLGPUBuffer();
        mBuffer1->init();

        Sphere sphere;
        sphere.create(0.7, 32, 32);
        sphere.fill(*(GLGPUBuffer*)mBuffer1);
        sphere.destroy();

        mBuffer2 = new GLGPUBuffer();
        mBuffer2->init();

        Torus torus;
        torus.create(3, 0.7, 32, 32);
        torus.fill(*(GLGPUBuffer*)mBuffer2);
        torus.destroy();

        material.setAmbientColor(Vector3f(0.92, 0.102, 0.16));
        material.setDiffuseColor(Vector3f(0.9, 0.11, 0.17));
        material.setSpecularColor(Vector3f(0.92, 0.872, 0.886));
        material.setShininess(6);
    }

    void onUpdate(FLOAT64 delta) override
    {
        addRotation(Vector3f(0.5,0.8,0.48), 0.018);

        gSceneManager->getRenderManager().queueTransformation(&mTransformation);
        gSceneManager->getRenderManager().queueBuffer(mBuffer1);
        material.use();

        gSceneManager->getRenderManager().queueTransformation(&mTransformation);
        gSceneManager->getRenderManager().queueBuffer(mBuffer2);
        material.use();
    }

    void onDestroy() override
    {
        SAFE_DELETE(mBuffer1);
        SAFE_DELETE(mBuffer2);
    }

private:

    GPUBuffer *mBuffer1;
    GPUBuffer *mBuffer2;
    BaseMaterial material;

};

class TestScene : public Scene
{
public:

    TestScene(const CStaticString &name) :
            Scene(name),
            camera(CNAME("MainCamera")),
            renderActor(CNAME("TestActor")),
            spotLight(CNAME("SpotLight1")),
            pointLight(CNAME("PointLight1")),
            ambientLight(CNAME("AmbientLight1")),
            directionalLight(CNAME("DirectionalLight1")),
            renderNodeActor(CNAME("RenderNodeActorTest"))
    {
        camera.setDirection(Vector3f(0,0,-1));
        camera.setPosition(Vector3f(0, 0, 10));
        camera.setOrientation(Vector3f(0,1,0));
        camera.setAutoAspectRatio(true);
        camera.setCinematicViewport(true);
        camera.setCinematicBorder(200);

        ambientLight.setLightIntensity(Vector3f(0.064, 0.064, 0.064));

        spotLight.setDirection(Vector3f(0,0,-1));
        spotLight.setPosition(Vector3f(0,0,8));
        spotLight.setCutoff(toRadians(13.0));
        spotLight.setInnerCutoff(toRadians(7.0));
        spotLight.setOuterCutoff(toRadians(13.0));
        spotLight.setAttenuationExponent(32);
        spotLight.setLightIntensity(Vector3f(0.91,0.870,0.940));

        pointLight.setPosition(Vector3f(3,3,3));
        pointLight.setRadius(10);
        pointLight.setConstantAttenuation(1);
        pointLight.setLinearAttenuation(0.08);
        pointLight.setQuadraticAttenuation(0.005);
        pointLight.setLightIntensity(Vector3f(0,0.1,0));

        directionalLight.setDirection(Vector3f(1,0,-1));
        directionalLight.setLightIntensity(Vector3f(0,0,0.1));

        getRoot().attachActor(&camera);
        //getRoot().attachActor(&renderActor);
        getRoot().attachActor(&spotLight);
        getRoot().attachActor(&ambientLight);
        getRoot().attachActor(&pointLight);
        getRoot().attachActor(&directionalLight);
        getRoot().attachActor(&renderNodeActor);
    }

    virtual ~TestScene() = default;

private:

    Camera camera;
    RenderActor renderActor;
    AmbientLight ambientLight;
    SpotLight spotLight;
    PointLight pointLight;
    DirectionalLight directionalLight;
    RenderNodeActor  renderNodeActor;
};

class TestApplication : public ApplicationContext
{
public:

    void setup() override
    {
        testScene = new TestScene(CNAME("GeometryTestScene"));
        getSceneManager()->addScene(testScene, true);
    }

    void close() override
    {
        SAFE_DELETE(testScene);
    }

private:

    TestScene *testScene;

};

void NewFragLightTesting()
{
    TestApplication application;

    application.init();
    application.run();
    application.destroy();
}

#endif //BERSERKENGINE_NEWFRAGMENTLIGHTNINGTESTING_H
