// don't compile; is broken
#ifdef BEGONIS

#include <core.h>
#include <entities.h>

    Tram::Tram(std::string_view& str){
        SetParameters(str);

        serializeddata = entityData.AddNew();
        Data* data = (Data*) serializeddata;

        data->model = PUID(str);
        data->collmodel = PUID(str);
        data->bogeycollmodel = PUID(str);
    };
    void Tram::UpdateParameters() {
        if (!is_loaded) return;
        rendercomponent->UpdateLocation(location);
        rendercomponent->UpdateRotation(rotation);
    };
    void Tram::Load(){
        Data* data = (Data*) serializeddata;

        rendercomponent = renderCompPool.AddNew();
        rendercomponent->SetModel(data->model);

        armaturecomponent = armatureCompPool.AddNew();
        armaturecomponent->SetModel(data->model);

        body_pcomp = physicsCompPool.AddNew();
        body_pcomp->SetParent(this);
        body_pcomp->SetModel(data->collmodel);
        body_pcomp->SetMass(1000.0f);
        body_pcomp->SetCollisionMask(-1 ^ COLL_WORLDOBJ);

        tramcomp = tramCompPool.AddNew();
        tramcomp->SetModel(data->model);
        tramcomp->SetParent(this);
        tramcomp->SetPhysicsComponent(body_pcomp);
        tramcomp->SetArmatureComponent(armaturecomponent);
        tramcomp->SetCollisionModels(data->bogeycollmodel, data->bogeycollmodel); // <---- put in the panto model

        rendercomponent->Init();
        body_pcomp->Init();
        armaturecomponent->Init();
        tramcomp->Init();


        rendercomponent->SetPose(armaturecomponent->GetPosePtr());

        is_loaded = true;

        UpdateParameters();
    }
    void Tram::Unload(){
        is_loaded = false;


        rendercomponent->Uninit();
        body_pcomp->Uninit();
        armaturecomponent->Uninit();
        tramcomp->Uninit();


        renderCompPool.Remove(rendercomponent);
        physicsCompPool.Remove(body_pcomp);
        armatureCompPool.Remove(armaturecomponent);
        tramCompPool.Remove(tramcomp);
        rendercomponent = nullptr;
        body_pcomp = nullptr;
        armaturecomponent = nullptr;
        tramcomp = nullptr;

    }
    void Tram::Serialize(){
        // TODO: make serialize code
    }

    void Tram::SerializeString(std::string& str){
        Data* data = (Data*) serializeddata;
        str = "tram ";
        ParametersString(str);
        str.append(ReverseUID(data->model));
        str.push_back(' ');
        str.append(ReverseUID(data->collmodel));
        str.push_back(' ');
        str.append(ReverseUID(data->bogeycollmodel));
    }
    void Tram::MessageHandler(Message& msg){
        //if (msg.type == MESSAGE_ACTIVATE && action) PListExec(action);
        if (msg.type == MESSAGE_ACTIVATE && tramcomp) tramcomp->Activate();
        return;
    }

#endif // BEGONIS