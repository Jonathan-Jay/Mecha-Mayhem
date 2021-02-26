#pragma once
#include "Utilities/ControllerInput.h"
#include "PhysBody.h"
#include <AudioEngine.h>
class Gun
{
public:
	Gun();
	enum class WEAPON {
		FIST,
		PISTOL,
		RIFLE,
		CANNON,
		MACHINEGUN,
		SHOTGUN//,
		//SWORD
	};
	struct GunProperties
	{
		char* name;
		int ammoCapacity;
		float damage;
		float cooldown;
		WEAPON type;
		bool isAuto = false;
	};
	struct GunPlayer 
	{
		int ammoLeft[2];
		WEAPON type[2];
		bool secondarySelected = false;
	};
	bool PickUpWeapon(WEAPON);
	void SwapWeapon(bool outOfAmmo = false);
	void UseWeapon(PhysBody& body, Transform& head, float offset);
	void ShootLazer(glm::quat offsetQuat, glm::quat rotation, glm::vec3 rayPos, glm::vec3 forwards, short damage);

	const static GunProperties pistol;
	const static GunProperties cannon;
	const static GunProperties rifle;
	const //GunProperties missileLauncher{ "Missile Launcher", 1, 100.f, 3.f };
	const static GunProperties shotgun;
	const static GunProperties machineGun;
private:

	GunPlayer playerWeapons {0, 0, WEAPON::FIST, WEAPON::FIST, false };
};

