#ifdef NEKAD
// for cell transition example:
    auto cell1 = PoolProxy<WorldCell>::New();
    auto cell2 = PoolProxy<WorldCell>::New();
    auto cell3 = PoolProxy<WorldCell>::New();
    auto cell4 = PoolProxy<WorldCell>::New();
    auto cell5 = PoolProxy<WorldCell>::New();
    cell1->SetName(UID("loader-test-cell1"));
    cell2->SetName(UID("loader-test-cell2"));
    cell3->SetName(UID("loader-test-cell3"));
    cell4->SetName(UID("loader-test-cell4"));
    cell5->SetName(UID("loader-test-cell5"));
    cell1->LoadFromDisk();
    cell2->LoadFromDisk();
    cell3->LoadFromDisk();
    cell4->LoadFromDisk();
    cell5->LoadFromDisk();
    cell1->AddLink(cell2);
    cell1->AddLink(cell3);
    cell1->AddLink(cell4);
    cell2->AddLink(cell1);
    cell3->AddLink(cell1);
    cell4->AddLink(cell1);
    cell4->AddLink(cell5);
    cell5->AddLink(cell4);

// for adding stuff example:
    // transition for the demo level
    //auto demo_trans = PoolProxy<WorldCell::Transition>::New();
    //demo_trans->AddPoint(glm::vec3(-5.0f, 0.0f, -5.0f));
    //demo_trans->AddPoint(glm::vec3(5.0f, 0.0f, -5.0f));
    //demo_trans->AddPoint(glm::vec3(-5.0f, 0.0f, 5.0f));
    //demo_trans->AddPoint(glm::vec3(5.0f, 0.0f, 5.0f));
    //demo_trans->AddPoint(glm::vec3(-5.0f, 5.0f, -5.0f));
    //demo_trans->AddPoint(glm::vec3(5.0f, 5.0f, -5.0f));
    //demo_trans->AddPoint(glm::vec3(-5.0f, 5.0f, 5.0f));
    //demo_trans->AddPoint(glm::vec3(5.0f, 5.0f, 5.0f));
    
    //demo_trans->GeneratePlanes();
    
    //demo->AddTransition(demo_trans);    
	
#endif