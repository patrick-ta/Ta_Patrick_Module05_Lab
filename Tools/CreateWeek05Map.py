import unreal as ue
actors=ue.get_editor_subsystem(ue.EditorActorSubsystem);level=ue.get_editor_subsystem(ue.LevelEditorSubsystem)
assert level.load_level('/Game/Course/Maps/L_Week04')
for a in actors.get_all_level_actors():
 if isinstance(a,(ue.TargetCrate,ue.CourseDoor,ue.InteractionPickup,ue.RechargeStation,ue.RunnerPickup)):actors.destroy_actor(a)
 if isinstance(a,ue.PlayerStart):a.set_actor_location(ue.Vector(500,0,40),False,False)
crate=ue.load_class(None,'/Game/Course/Week04/BP_TargetCrate.BP_TargetCrate_C')
actors.spawn_actor_from_class(crate,ue.Vector(380,0,34)).set_actor_label('Combat target - 100 health')
world=ue.get_editor_subsystem(ue.UnrealEditorSubsystem).get_editor_world()
world.get_world_settings().set_editor_property('default_game_mode',ue.load_class(None,'/Game/Course/Week05/BP_CombatGameMode.BP_CombatGameMode_C'))
assert ue.EditorLoadingAndSavingUtils.save_map(world,'/Game/Course/Maps/L_Week05')
ue.log('WEEK5_MAP_CREATED')
