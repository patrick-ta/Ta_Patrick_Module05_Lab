import unreal as u
import json,os
rows=[]
def check(name,ok):
 rows.append({'check':name,'result':'PASS' if ok else 'FAIL'})
def default(name):
 return u.get_default_object(u.load_class(None,'/Game/Course/Week05/'+name+'.'+name+'_C'))
p=default('BP_Projectile');w=default('BP_Weapon');g=default('BP_CombatGameMode')
check('Projectile_Lifetime2',abs(p.get_editor_property('lifetime')-2)<.001)
c=p.get_editor_property('collision');v=p.get_editor_property('visual');m=p.get_editor_property('movement')
check('Projectile_SphereRoot',p.get_editor_property('root_component')==c and isinstance(c,u.SphereComponent))
check('Projectile_SpriteAssigned',v.get_sprite() is not None)
check('Projectile_SpriteNoCollision',v.get_collision_enabled()==u.CollisionEnabled.NO_COLLISION)
check('Projectile_UpdatedCollision',m.get_editor_property('updated_component')==c)
check('Projectile_GravityZero',m.get_editor_property('projectile_gravity_scale')==0)
check('Projectile_Sweep',m.get_editor_property('sweep_collision'))
for prop,value in [('cooldown',.25),('damage',25),('projectile_speed',600),('ammo',20)]:
 check('Weapon_'+prop,abs(w.get_editor_property(prop)-value)<.001)
check('Weapon_ProjectileChild',w.get_editor_property('projectile_class')==p.get_class())
check('Mode_PawnChild','BP_RunnerCharacter_C' in str(g.get_editor_property('default_pawn_class')))
world=u.EditorLoadingAndSavingUtils.load_map('/Game/Course/Maps/L_Week05')
check('Map_Loads',world is not None)
check('Map_GameMode','BP_CombatGameMode_C' in str(world.get_world_settings().get_editor_property('default_game_mode')))
actors=u.get_editor_subsystem(u.EditorActorSubsystem).get_all_level_actors()
check('Map_Target',any('BP_TargetCrate' in a.get_class().get_name() for a in actors))
check('Map_PlayerStart',any(isinstance(a,u.PlayerStart) for a in actors))
path=os.path.join(u.Paths.project_saved_dir(),'Verification','Asset_Audit.json')
os.makedirs(os.path.dirname(path),exist_ok=True)
with open(path,'w') as f:json.dump(rows,f,indent=2)
assert all(r['result']=='PASS' for r in rows),str(rows)
u.log('WEEK5_ASSET_AUDIT_PASS '+str(len(rows)))
