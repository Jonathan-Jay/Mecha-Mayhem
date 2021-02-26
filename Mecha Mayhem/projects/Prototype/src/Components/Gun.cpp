#include "Gun.h"

const Gun::GunProperties Gun::pistol = { "Pistol", 30, 5.f, 0.5f, WEAPON::PISTOL };
const Gun::GunProperties Gun::cannon{ "Proton Cannon", 3, 30.f, 2.f, WEAPON::CANNON };
const Gun::GunProperties Gun::rifle{ "Proton Rifle", 20, 10.f, 0.8f, WEAPON::RIFLE };
const Gun::GunProperties Gun::shotgun{ "Shotgun", 15, 25.f, 2.f, WEAPON::SHOTGUN };
const Gun::GunProperties Gun::machineGun{ "Machine Gun", 50, 5.f, 0.1f, WEAPON::MACHINEGUN, true };

Gun::Gun()
{
}

bool Gun::PickUpWeapon(WEAPON pickup)
{
	//if this is false -> primary is selected
	bool selected = !playerWeapons.secondarySelected;
	if (playerWeapons.type[selected] == WEAPON::FIST) 
	{
		playerWeapons.type[selected] = pickup;
		//based on weapon, add ammo
		switch (pickup)
		{
		case WEAPON::PISTOL:
			playerWeapons.ammoLeft[selected] = pistol.ammoCapacity;
			break;
		case WEAPON::RIFLE:
			playerWeapons.ammoLeft[selected] = rifle.ammoCapacity;
			break;
		case WEAPON::SHOTGUN:
			playerWeapons.ammoLeft[selected] = shotgun.ammoCapacity;
			break;
		case WEAPON::MACHINEGUN:
			playerWeapons.ammoLeft[selected] = machineGun.ammoCapacity;
			break;
		case WEAPON::CANNON:
			playerWeapons.ammoLeft[selected] = cannon.ammoCapacity;
			break;
		default:
			playerWeapons.ammoLeft[selected] = 20;
			break;
		}
		playerWeapons.secondarySelected = !playerWeapons.secondarySelected;
		//m_swapWeapon.play();
		AudioEngine::Instance().GetEvent("pickup").Restart();

		return true;
	}
	return false;
}

void Gun::SwapWeapon(bool outOfAmmo)
{
	AudioEngine::Instance().GetEvent("reload").Restart();
	if (outOfAmmo)
	{
		playerWeapons.type[playerWeapons.secondarySelected] = Gun::WEAPON::FIST;
	}
	playerWeapons.secondarySelected = !playerWeapons.secondarySelected;
}

void Gun::UseWeapon(PhysBody& body, Transform& head, float offset)
{
	switch (playerWeapons.type[playerWeapons.secondarySelected]) {
	case Gun::WEAPON::FIST:
		/*if (m_grounded)
			body.SetVelocity(BLM::BTtoGLM(Melee(BLM::BTtoGLM(body.GetBody()->getWorldTransform().getOrigin())
				- head.GetForwards())));*/
		break;
	case Gun::WEAPON::PISTOL:
	{
		m_weaponCooldown = Gun::pistol.cooldown;

		AudioEngine::Instance().GetEvent("shoot").Restart();
		glm::quat offsetQuat = glm::angleAxis(offset, glm::vec3(0.24253f, 0.97014f, 0.f));
		ShootLazer(offsetQuat, head.GetGlobalRotation(), head.GetGlobalPosition() +
			glm::vec3(m_gunOffset * glm::rotate(glm::mat4(1.f), m_rot.y, BLM::GLMup)),
			glm::rotate(offsetQuat, -head.GetForwards()), 3);
		//deal with ammo here
		if (--m_currWeaponAmmo <= 0) {
			Gun::SwapWeapon(true);
		}
		break;
	}
	case Gun::WEAPON::RIFLE:
	{
		m_weaponCooldown = Gun::rifle.cooldown;

		AudioEngine::Instance().GetEvent("shoot").Restart();
		glm::quat offsetQuat = glm::angleAxis(offset, glm::vec3(0.24253f, 0.97014f, 0.f));
		ShootLazer(offsetQuat, head.GetGlobalRotation(), head.GetGlobalPosition() +
			glm::vec3(m_gunOffset * glm::rotate(glm::mat4(1.f), m_rot.y, BLM::GLMup)),
			glm::rotate(offsetQuat, -head.GetForwards()), 3);
		//deal with ammo here
		if (--m_currWeaponAmmo <= 0) {
			Gun::SwapWeapon(true);
		}
		break;
	}
	case Gun::WEAPON::SHOTGUN:
	{
		m_weaponCooldown = Gun::shotgun.cooldown;

		AudioEngine::Instance().GetEvent("shoot").Restart();
		glm::quat offsetQuat = glm::angleAxis(offset, glm::vec3(0.24253f, 0.97014f, 0.f));
		ShootLazer(offsetQuat, head.GetGlobalRotation(), head.GetGlobalPosition() +
			glm::vec3(m_gunOffset * glm::rotate(glm::mat4(1.f), m_rot.y, BLM::GLMup)),
			glm::rotate(offsetQuat, -head.GetForwards()), 3);
		//deal with ammo here
		if (--m_currWeaponAmmo <= 0) {
			Gun::SwapWeapon(true);
		}
		break;
	}
	case Gun::WEAPON::MACHINEGUN:
	{
		m_weaponCooldown = Gun::machineGun.cooldown;

		AudioEngine::Instance().GetEvent("shoot").Restart();
		glm::quat offsetQuat = glm::angleAxis(offset, glm::vec3(0.24253f, 0.97014f, 0.f));
		ShootLazer(offsetQuat, head.GetGlobalRotation(), head.GetGlobalPosition() +
			glm::vec3(m_gunOffset * glm::rotate(glm::mat4(1.f), m_rot.y, BLM::GLMup)),
			glm::rotate(offsetQuat, -head.GetForwards()), 3);
		//deal with ammo here
		if (--m_currWeaponAmmo <= 0) {
			Gun::SwapWeapon(true);
		}
		break;
	}
	case Gun::WEAPON::CANNON:
	{
		m_weaponCooldown = Gun::cannon.cooldown;

		AudioEngine::Instance().GetEvent("shoot").Restart();
		glm::quat offsetQuat = glm::angleAxis(offset, glm::vec3(0.24253f, 0.97014f, 0.f));
		ShootLazer(offsetQuat, head.GetGlobalRotation(), head.GetGlobalPosition() +
			glm::vec3(m_gunOffset * glm::rotate(glm::mat4(1.f), m_rot.y, BLM::GLMup)),
			glm::rotate(offsetQuat, -head.GetForwards()), 3);
		//deal with ammo here
		if (--m_currWeaponAmmo <= 0) {
			Gun::SwapWeapon(true);
		}
		break;
	}
	default:	//break;	for demo, all guns do the same
	//case WEAPON::PISTOL:
	{
		m_weaponCooldown = 0.25f;

		//m_shootLaser.play();
		AudioEngine::Instance().GetEvent("shoot").Restart();

		glm::quat offsetQuat = glm::angleAxis(offset, glm::vec3(0.24253f, 0.97014f, 0.f));
		ShootLazer(offsetQuat, head.GetGlobalRotation(), head.GetGlobalPosition() +
			glm::vec3(m_gunOffset * glm::rotate(glm::mat4(1.f), m_rot.y, BLM::GLMup)),
			glm::rotate(offsetQuat, -head.GetForwards()), 3);

		//deal with ammo here
		if (--m_currWeaponAmmo <= 0) {
			Gun::SwapWeapon(true);
		}
	}
	break;
	}

	void ShootLazer(glm::quat offsetQuat, glm::quat rotation, glm::vec3 rayPos, glm::vec3 forwards, short damage)
	{
		RayResult p = PhysBody::GetRaycastResult(BLM::GLMtoBT(rayPos),
			BLM::GLMtoBT(forwards * 2000.f));
		if (p.hasHit())
		{
			entt::entity playerIdTest = p.m_collisionObject->getUserIndex();
			if (ECS::Exists(playerIdTest)) {
				if (ECS::HasComponent<Player>(playerIdTest)) {
					if (ECS::GetComponent<Player>(playerIdTest).TakeDamage(damage))
						++m_killCount;
				}
			}
			Rendering::effects->ShootLaser(rotation * offsetQuat, rayPos,
				glm::length(BLM::BTtoGLM(p.m_hitPointWorld) - rayPos));
		}
		else {
			Rendering::effects->ShootLaser(rotation * offsetQuat, rayPos, 2000.f);
		}
	}
}
