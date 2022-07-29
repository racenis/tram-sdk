// don't compile; is broken
#ifdef BEGONIS

#include <core.h>
#include <entities.h>
#include <physics.h>

    Bus::Bus(std::string_view& str){
        SetParameters(str);

        serializeddata = entityData.AddNew();
        Data* data = (Data*) serializeddata;

        data->model = PUID(str);
        data->collmodel = PUID(str);
        data->wheel_collmodel = PUID(str);
    };
    void Bus::UpdateParameters() {
        if (!isloaded) return;
        rendercomponent->UpdateLocation(location);
        rendercomponent->UpdateRotation(rotation);
    };
    void Bus::Load(){
        Data* data = (Data*) serializeddata;

        rendercomponent = renderCompPool.AddNew();
        rendercomponent->SetModel(data->model);

        armaturecomponent = armatureCompPool.AddNew();
        armaturecomponent->SetModel(data->model);

        body_pcomp = physicsCompPool.AddNew();
        body_pcomp->SetParent(this);
        body_pcomp->SetModel(data->collmodel);
        body_pcomp->SetMass(1670.0f);
        body_pcomp->SetOffset(glm::vec3(0.0f, -1.0f, 0.0f));
        body_pcomp->SetStartAsleep();
        //body_pcomp->SetCollisionMask(-1 ^ COLL_PLAYER);

        buscomp = busCompPool.AddNew();
        buscomp->SetModel(data->model);
        buscomp->SetParent(this);
        buscomp->SetPhysicsComponent(body_pcomp);
        buscomp->SetArmatureComponent(armaturecomponent);


        rendercomponent->Init();
        body_pcomp->Init();
        armaturecomponent->Init();
        buscomp->Init();

        //body_pcomp->SetLinearFactor(0.0f);

        rendercomponent->SetPose(armaturecomponent->GetPosePtr());

        isloaded = true;

        UpdateParameters();
    }
    void Bus::Unload(){
        isloaded = false;


        rendercomponent->Uninit();
        buscomp->Uninit();
        body_pcomp->Uninit();
        armaturecomponent->Uninit();



        renderCompPool.Remove(rendercomponent);
        physicsCompPool.Remove(body_pcomp);
        armatureCompPool.Remove(armaturecomponent);
        busCompPool.Remove(buscomp);
        rendercomponent = nullptr;
        body_pcomp = nullptr;
        armaturecomponent = nullptr;
        buscomp = nullptr;

    }
    void Bus::Serialize(){
        // TODO: make serialize code
    }

    void Bus::SerializeString(std::string& str){
        Data* data = (Data*) serializeddata;
        str = "bus ";
        ParametersString(str);
        str.append(ReverseUID(data->model));
        str.push_back(' ');
        str.append(ReverseUID(data->collmodel));
        str.push_back(' ');
        str.append(ReverseUID(data->wheel_collmodel));
    }
    void Bus::MessageHandler(Message& msg){
        //if (msg.type == MESSAGE_ACTIVATE && action) PListExec(action);
        if (msg.type == MESSAGE_ACTIVATE && buscomp) buscomp->Activate();
        return;
    }
#endif