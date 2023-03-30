// don't compile; is broken
#ifdef BEGONIS

#include <core.h>
#include <entities.h>

#include <etc.h>



    Mongus::Mongus(std::string_view& str){
        SetParameters(str);

        serializeddata = entityData.AddNew();
        Data* data = (Data*) serializeddata;

        std::cout << "mongus ctr" << str << std::endl;

        data->mtype = str.find("bingus") != std::string_view::npos ? MONGUS_BINGUS : str.find("floppa") != std::string_view::npos ? MONGUS_FLOPPA : MONGUS_MONGUS;
    };
    Mongus::Mongus(const char* nname, glm::vec3 pos, glm::vec3 rot){
        name = UID(nname);
        location = pos;
        rotation = glm::quat(rot);

        //serializeddata = entityData.AddNew();
        //Data* data = (Data*) serializeddata;

        //data->model = UID(modelname);
        //data->collmodel = UID(collisionmodelname);
    }
    void Mongus::UpdateParameters() {
        if (!is_loaded) return;
        rendercomponent->UpdateLocation(location);
        rendercomponent->UpdateRotation(rotation);
        rendercomponent->SetCellParams(cell->HasInteriorLighting());
        //std::cout << "crate";
        //if(isloaded) light->UpdateLocation(location.x, location.y, location.z);
        //rendComp.UpdateLocation(transf[0], transf[1], transf[2]);
        //rendComp.UpdateRotation(rotat[0], rotat[1], rotat[2]);
    };
    void Mongus::Load(){
        Data* data = (Data*) serializeddata;

        rendercomponent = renderCompPool.AddNew();
        armaturecomponent = armatureCompPool.AddNew();
        controllercomponent = controllerCompPool.AddNew();
        aicomponent = AICompPool.AddNew();
        physicscomponent = physicsCompPool.AddNew();

        if (data->mtype == MONGUS_FLOPPA){
            armaturecomponent->SetModel(UID("floppa"));
            rendercomponent->SetModel(UID("floppa"));

            controllercomponent->SetAnimation(ACTION_IDLE, UID("floppa-idle"));
        } else if (data->mtype == MONGUS_BINGUS){
            armaturecomponent->SetModel(UID("bingus"));
            rendercomponent->SetModel(UID("bingus"));

            physicscomponent->SetOffset(glm::vec3(0.0f, 0.4f, 0.5f));

            controllercomponent->SetAnimation(ACTION_IDLE, UID("bingus-idle"));
        } else {
            armaturecomponent->SetModel(UID("mongus"));
            rendercomponent->SetModel(UID("mongus"));

            physicscomponent->SetOffset(glm::vec3(0.0f, 0.6f, 0.0f));

            controllercomponent->SetAnimation(ACTION_IDLE, UID("Sway"));
            controllercomponent->SetAnimation(ACTION_MOVE_FORWARD_VERY_FAST, UID("Run"));
        }



        physicscomponent->SetParent(this);
        physicscomponent->SetShapeCapsule(0.5f, 0.25f);
        physicscomponent->SetMass(40.0f);
        physicscomponent->SetStartAsleep();


        controllercomponent->SetPhysicsComponent(physicscomponent);
        controllercomponent->SetArmatureComponent(armaturecomponent);




        aicomponent->SetController(controllercomponent);

        //entityData.Remove(serializeddata);
        //serializeddata = nullptr;

        rendercomponent->Init();
        physicscomponent->Init();
        controllercomponent->Init();
        armaturecomponent->Init();
        aicomponent->Init();





        rendercomponent->SetPose(armaturecomponent->GetPosePtr());
        physicscomponent->DisableRotation();

        if (data->mtype == MONGUS_BINGUS) physicscomponent->DisableMovement();

        is_loaded = true;

        UpdateParameters();
    }
    void Mongus::Unload(){
        is_loaded = false;

        Serialize();

        rendercomponent->Uninit();
        physicscomponent->Uninit();
        controllercomponent->Uninit();
        armaturecomponent->Uninit();
        aicomponent->Uninit();


        renderCompPool.Remove(rendercomponent);
        physicsCompPool.Remove(physicscomponent);
        controllerCompPool.Remove(controllercomponent);
        armatureCompPool.Remove(armaturecomponent);
        AICompPool.Remove(aicomponent);
        rendercomponent = nullptr;
        physicscomponent = nullptr;
        controllercomponent = nullptr;
        armaturecomponent = nullptr;
        aicomponent = nullptr;

    }
    void Mongus::Serialize(){
        serializeddata = entityData.AddNew();
        Data* data = (Data*) serializeddata;

        if (rendercomponent->GetModel() == UID("floppa"))
            data->mtype = MONGUS_FLOPPA;
        else if (rendercomponent->GetModel() == UID("bingus"))
            data->mtype = MONGUS_BINGUS;
        else
            data->mtype = MONGUS_MONGUS;
    }

    void Mongus::SerializeString(std::string& str){
        Data* data = (Data*) serializeddata;
        str = "mongus ";
        ParametersString(str);
        if (data->mtype == MONGUS_FLOPPA)
            str.append(" floppa");
        else if (data->mtype == MONGUS_BINGUS)
            str.append(" bingus");
        else
            str.append(" mongus");
    }
    void Mongus::MessageHandler(Message& msg){
        /*if (msg.type == MESSAGE_ACTIVATE && rendercomponent->GetModel() == UID("bingus")) bingus_dialog.emitQuestions();
        if (msg.type == MESSAGE_ACTIVATE && rendercomponent->GetModel() == UID("floppa")) floppa_dialog.emitQuestions();*/
        if (msg.type == MESSAGE_ACTIVATE && rendercomponent->GetModel() == UID("mongus")) aicomponent->SetFlag(AI_RUN_IN_CIRCLES, true);
        return;
    }

#endif