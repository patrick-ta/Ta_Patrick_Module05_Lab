import unreal as ue
from pathlib import Path
import json
checks=[]
def check(name,passed):
 checks.append({'name':name,'passed':bool(passed)})
 assert passed,name
assets=ue.EditorAssetLibrary
folder='/Game/Course/Week04/'
crate=ue.load_class(None,folder+'BP_TargetCrate.BP_TargetCrate_C')
mode=ue.load_class(None,folder+'BP_Week04GameMode.BP_Week04GameMode_C')
check('Crate Blueprint loads',crate is not None)
check('GameMode Blueprint loads',mode is not None)
runner=ue.load_class(None,'/Game/Course/Week03/BP_RunnerCharacter.BP_RunnerCharacter_C')
check('GameMode spawns configured Runner',ue.get_default_object(mode).get_editor_property('default_pawn_class')==runner)
rd=ue.get_default_object(runner)
for prop in ['move_action','jump_action','gameplay_context','idle_animation','run_animation']:
 check('Runner '+prop+' assigned',rd.get_editor_property(prop) is not None)
check('Player has HealthComponent',rd.get_component_by_class(ue.HealthComponent) is not None)
check('Player has InteractionComponent',rd.get_component_by_class(ue.InteractionComponent) is not None)
level=ue.get_editor_subsystem(ue.LevelEditorSubsystem)
check('Week04 map reopens',level.load_level('/Game/Course/Maps/L_Week04'))
actors=ue.get_editor_subsystem(ue.EditorActorSubsystem).get_all_level_actors()
for cls in [ue.TargetCrate,ue.CourseRoom,ue.PlayerStart,ue.CourseDoor,ue.InteractionPickup,ue.RechargeStation]:
 check('One '+cls.__name__,sum(isinstance(a,cls) for a in actors)==1)
world=ue.get_editor_subsystem(ue.UnrealEditorSubsystem).get_editor_world()
check('World mode set',world.get_world_settings().get_editor_property('default_game_mode')==mode)
room=next(a for a in actors if isinstance(a,ue.CourseRoom))
check('Blue foreground nonblocking',room.get_editor_property('foreground').get_collision_enabled()==ue.CollisionEnabled.NO_COLLISION)
for prop in ['floor_collision','left_wall_collision','right_wall_collision']:
 check(prop+' BlockAll',str(room.get_editor_property(prop).get_collision_profile_name())=='BlockAll')
p=Path(ue.Paths.project_dir())/'Saved/Verification';p.mkdir(parents=True,exist_ok=True)
(p/'assets.json').write_text(json.dumps(checks,indent=2))
ue.log('WEEK4_ASSET_CHECKS_PASSED: '+str(len(checks)))
