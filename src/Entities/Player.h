
#pragma once

#include "Pawn.h"
#include "../Inventory.h"
#include "../Defines.h"
#include "../World.h"
#include "../ClientHandle.h"

#include "../Statistics.h"





class cGroup;
class cWindow;
class cClientHandle;
class cTeam;





// tolua_begin
class cPlayer :
	public cPawn
{
	typedef cPawn super;
	
public:
	enum
	{
		MAX_HEALTH = 20,
		MAX_FOOD_LEVEL = 20,
		EATING_TICKS = 30,  ///< Number of ticks it takes to eat an item
	} ;
	// tolua_end
	
	CLASS_PROTODEF(cPlayer)
	

	cPlayer(cClientHandle * a_Client, const AString & a_PlayerName);
	virtual ~cPlayer();

	virtual void SpawnOn(cClientHandle & a_Client) override;
	
	virtual void Tick(float a_Dt, cChunk & a_Chunk) override;

	virtual void HandlePhysics(float a_Dt, cChunk &) override { UNUSED(a_Dt); };

	/** Returns the curently equipped weapon; empty item if none */
	virtual cItem GetEquippedWeapon(void) const override { return m_Inventory.GetEquippedItem(); }
	
	/** Returns the currently equipped helmet; empty item if none */
	virtual cItem GetEquippedHelmet(void) const override { return m_Inventory.GetEquippedHelmet(); }
	
	/** Returns the currently equipped chestplate; empty item if none */
	virtual cItem GetEquippedChestplate(void) const override { return m_Inventory.GetEquippedChestplate(); }

	/** Returns the currently equipped leggings; empty item if none */
	virtual cItem GetEquippedLeggings(void) const override { return m_Inventory.GetEquippedLeggings(); }
	
	/** Returns the currently equipped boots; empty item if none */
	virtual cItem GetEquippedBoots(void) const override { return m_Inventory.GetEquippedBoots(); }


	// tolua_begin

	/** Sets the experience total
	Returns true on success
	"should" really only be called at init or player death, plugins excepted
	*/
	bool SetCurrentExperience(short a_XpTotal);

	/* changes Xp by Xp_delta, you "shouldn't" inc more than MAX_EXPERIENCE_ORB_SIZE
	Wont't allow xp to go negative
	Returns the new current experience, -1 on error
	*/
	short DeltaExperience(short a_Xp_delta);

	/** Gets the experience total - XpTotal for score on death */
	inline short GetXpLifetimeTotal(void) { return m_LifetimeTotalXp; }

	/** Gets the currrent experience */
	inline short GetCurrentXp(void) { return m_CurrentXp; }

	/** Gets the current level - XpLevel */
	short GetXpLevel(void);

	/** Gets the experience bar percentage - XpP */
	float GetXpPercentage(void);

	/** Caculates the amount of XP needed for a given level
	Ref: http://minecraft.gamepedia.com/XP
	*/
	static short XpForLevel(short int a_Level);

	/** Inverse of XpForLevel
	Ref: http://minecraft.gamepedia.com/XP
	values are as per this with pre-calculations
	*/
	static short CalcLevelFromXp(short int a_CurrentXp);

	// tolua_end
	
	/** Starts charging the equipped bow */
	void StartChargingBow(void);
	
	/** Finishes charging the current bow. Returns the number of ticks for which the bow has been charged */
	int FinishChargingBow(void);
	
	/** Cancels the current bow charging */
	void CancelChargingBow(void);
	
	/** Returns true if the player is currently charging the bow */
	bool IsChargingBow(void) const { return m_IsChargingBow; }

	void SetTouchGround( bool a_bTouchGround );
	inline void SetStance( const double a_Stance ) { m_Stance = a_Stance; }
	double GetEyeHeight(void) const;													// tolua_export
	Vector3d GetEyePosition(void) const;												// tolua_export
	inline bool IsOnGround(void) const {return m_bTouchGround; }  // tolua_export
	inline double GetStance(void) const { return GetPosY() + 1.62; }  // tolua_export  // TODO: Proper stance when crouching etc.
	inline cInventory &       GetInventory(void)       { return m_Inventory; }	// tolua_export
	inline const cInventory & GetInventory(void) const { return m_Inventory; }
	
	inline const cItem & GetEquippedItem(void) const { return GetInventory().GetEquippedItem(); }  // tolua_export

	/** Returns whether the player is climbing (ladders, vines e.t.c). */
	bool IsClimbing(void) const;

	virtual void TeleportToCoords(double a_PosX, double a_PosY, double a_PosZ) override;

	// tolua_begin
	
	/** Sends the "look" packet to the player, forcing them to set their rotation to the specified values.
	a_YawDegrees is clipped to range [-180, +180),
	a_PitchDegrees is clipped to range [-180, +180) but the client only uses [-90, +90]
	*/
	void SendRotation(double a_YawDegrees, double a_PitchDegrees);
	
	/** Returns the position where projectiles thrown by this player should start, player eye position + adjustment */
	Vector3d GetThrowStartPos(void) const;
	
	/** Returns the initial speed vector of a throw, with a 3D length of a_SpeedCoeff. */
	Vector3d GetThrowSpeed(double a_SpeedCoeff) const;
	
	/** Returns the current gamemode. Partly OBSOLETE, you should use IsGameModeXXX() functions wherever applicable */
	eGameMode GetGameMode(void) const { return m_GameMode; }
	
	/** Returns the current effective gamemode (inherited gamemode is resolved before returning) */
	eGameMode GetEffectiveGameMode(void) const { return (m_GameMode == gmNotSet) ? m_World->GetGameMode() : m_GameMode; }
	
	/** Sets the gamemode for the player.
	The gamemode may be gmNotSet, in that case the player inherits the world's gamemode.
	Updates the gamemode on the client (sends the packet)
	*/
	void SetGameMode(eGameMode a_GameMode);

	/** Returns true if the player is in Creative mode, either explicitly, or by inheriting from current world */
	bool IsGameModeCreative(void) const;
	
	/** Returns true if the player is in Survival mode, either explicitly, or by inheriting from current world */
	bool IsGameModeSurvival(void) const;
	
	/** Returns true if the player is in Adventure mode, either explicitly, or by inheriting from current world */
	bool IsGameModeAdventure(void) const;
	
	AString GetIP(void) const { return m_IP; }  // tolua_export

	/** Returns the associated team, NULL if none */
	cTeam * GetTeam(void) { return m_Team; } // tolua_export

	/** Sets the player team, NULL if none */
	void SetTeam(cTeam * a_Team);

	/** Forces the player to query the scoreboard for his team */
	cTeam * UpdateTeam(void);

	// tolua_end

	/** Return the associated statistic and achievement manager. */
	cStatManager & GetStatManager() { return m_Stats; }

	/** Awards the player an achievement.
	If all prerequisites are met, this method will award the achievement and will broadcast a chat message.
	If the achievement has been already awarded to the player, this method will just increment the stat counter.
	Returns the _new_ stat value. (0 = Could not award achievement) */
	unsigned int AwardAchievement(const eStatistic a_Ach);
	
	void SetIP(const AString & a_IP);
	
	// Sets the current gamemode, doesn't check validity, doesn't send update packets to client
	void LoginSetGameMode(eGameMode a_GameMode);

	/** Forces the player to move in the given direction. 
	 *  @deprecated Use SetSpeed instead.
	 */
	void ForceSetSpeed(const Vector3d & a_Speed); // tolua_export

	/** Sets the speed of the player and moves them in the given speed. */
	void SetSpeed           (const Vector3d & a_Speed);
	virtual void SetSpeed   (double a_SpeedX, double a_SpeedY, double a_SpeedZ) override;
	virtual void SetSpeedX  (double a_SpeedX) override;
	virtual void SetSpeedY  (double a_SpeedY) override;
	virtual void SetSpeedZ  (double a_SpeedZ) override;

	/** Tries to move to a new position, with attachment-related checks (y == -999) */
	void MoveTo(const Vector3d & a_NewPos);  // tolua_export

	cWindow * GetWindow(void) { return m_CurrentWindow; }  // tolua_export
	const cWindow * GetWindow(void) const { return m_CurrentWindow; }
	
	/** Opens the specified window; closes the current one first using CloseWindow() */
	void OpenWindow(cWindow * a_Window);  // Exported in ManualBindings.cpp
	
	// tolua_begin
	
	/** Closes the current window, resets current window to m_InventoryWindow. A plugin may refuse the closing if a_CanRefuse is true */
	void CloseWindow(bool a_CanRefuse = true);
	
	/** Closes the current window if it matches the specified ID, resets current window to m_InventoryWindow */
	void CloseWindowIfID(char a_WindowID, bool a_CanRefuse = true);

	cClientHandle * GetClientHandle(void) const { return m_ClientHandle; }

	void SendMessage          (const AString & a_Message) { m_ClientHandle->SendChat(a_Message, mtCustom); }
	void SendMessageInfo      (const AString & a_Message) { m_ClientHandle->SendChat(a_Message, mtInformation); }
	void SendMessageFailure   (const AString & a_Message) { m_ClientHandle->SendChat(a_Message, mtFailure); }
	void SendMessageSuccess   (const AString & a_Message) { m_ClientHandle->SendChat(a_Message, mtSuccess); }
	void SendMessageWarning   (const AString & a_Message) { m_ClientHandle->SendChat(a_Message, mtWarning); }
	void SendMessageFatal     (const AString & a_Message) { m_ClientHandle->SendChat(a_Message, mtFailure); }
	void SendMessagePrivateMsg(const AString & a_Message, const AString & a_Sender) { m_ClientHandle->SendChat(a_Message, mtPrivateMessage, a_Sender); }
	void SendMessage          (const cCompositeChat & a_Message) { m_ClientHandle->SendChat(a_Message); }

	const AString & GetName(void) const { return m_PlayerName; }
	void SetName(const AString & a_Name) { m_PlayerName = a_Name; }
	
	// tolua_end

	typedef std::list< cGroup* > GroupList;
	typedef std::list< std::string > StringList;

	/** Adds a player to existing group or creates a new group when it doesn't exist */
	void AddToGroup( const AString & a_GroupName );							// tolua_export
	
	/** Removes a player from the group, resolves permissions and group inheritance (case sensitive) */
	void RemoveFromGroup( const AString & a_GroupName );					// tolua_export
	
	bool HasPermission( const AString & a_Permission );						// tolua_export
	const GroupList & GetGroups() { return m_Groups; }						// >> EXPORTED IN MANUALBINDINGS <<
	StringList GetResolvedPermissions();									// >> EXPORTED IN MANUALBINDINGS <<
	bool IsInGroup( const AString & a_Group );								// tolua_export

	// tolua_begin
	
	/** Returns the full color code to use for this player, based on their primary group or set in m_Color.
	The returned value includes the cChatColor::Delimiter. */
	AString GetColor(void) const;

	/** tosses the item in the selected hotbar slot */
	void TossEquippedItem(char a_Amount = 1);

	/** tosses the item held in hand (when in UI windows) */
	void TossHeldItem(char a_Amount = 1);

	/** tosses a pickup newly created from a_Item */
	void TossPickup(const cItem & a_Item);

	/** Heals the player by the specified amount of HPs (positive only); sends health update */
	void Heal(int a_Health);
	
	int    GetFoodLevel                 (void) const { return m_FoodLevel; }
	double GetFoodSaturationLevel       (void) const { return m_FoodSaturationLevel; }
	int    GetFoodTickTimer             (void) const { return m_FoodTickTimer; }
	double GetFoodExhaustionLevel       (void) const { return m_FoodExhaustionLevel; }
	int    GetFoodPoisonedTicksRemaining(void) const { return m_FoodPoisonedTicksRemaining; }
	
	/** Returns true if the player is satiated, i. e. their foodlevel is at the max and they cannot eat anymore */
	bool IsSatiated(void) const { return (m_FoodLevel >= MAX_FOOD_LEVEL); }
	
	void SetFoodLevel                 (int a_FoodLevel);
	void SetFoodSaturationLevel       (double a_FoodSaturationLevel);
	void SetFoodTickTimer             (int a_FoodTickTimer);
	void SetFoodExhaustionLevel       (double a_FoodExhaustionLevel);
	void SetFoodPoisonedTicksRemaining(int a_FoodPoisonedTicksRemaining);

	/** Adds to FoodLevel and FoodSaturationLevel, returns true if any food has been consumed, false if player "full" */
	bool Feed(int a_Food, double a_Saturation);

	/** Adds the specified exhaustion to m_FoodExhaustion. Expects only positive values. */
	void AddFoodExhaustion(double a_Exhaustion)
	{
		m_FoodExhaustionLevel += a_Exhaustion;
	}
	
	/** Starts the food poisoning for the specified amount of ticks; if already foodpoisoned, sets FoodPoisonedTicksRemaining to the larger of the two */
	void FoodPoison(int a_NumTicks);
	
	/** Returns true if the player is currently in the process of eating the currently equipped item */
	bool IsEating(void) const { return (m_EatingFinishTick >= 0); }
	
	/** Returns true if the player is currently flying. */
	bool IsFlying(void) const { return m_IsFlying; }

	/** Returns if a player is sleeping in a bed */
	bool IsInBed(void) const { return m_bIsInBed; }

	/** returns true if the player has thrown out a floater. */
	bool IsFishing(void) const { return m_IsFishing; }

	void SetIsFishing(bool a_IsFishing, int a_FloaterID = -1) { m_IsFishing = a_IsFishing; m_FloaterID = a_FloaterID; }

	int GetFloaterID(void) const { return m_FloaterID; }

	// tolua_end

	/** Sets a player's in-bed state; we can't be sure plugins will keep this value updated, so no exporting */
	void SetIsInBed(bool a_Flag) { m_bIsInBed = a_Flag; }
	
	/** Starts eating the currently equipped item. Resets the eating timer and sends the proper animation packet */
	void StartEating(void);
	
	/** Finishes eating the currently equipped item. Consumes the item, updates health and broadcasts the packets */
	void FinishEating(void);
	
	/** Aborts the current eating operation */
	void AbortEating(void);
	
	virtual void KilledBy(cEntity * a_Killer) override;

	virtual void Killed(cEntity * a_Victim) override;
	
	void Respawn(void);															// tolua_export

	void SetVisible( bool a_bVisible );										// tolua_export
	bool IsVisible(void) const { return m_bVisible; }									// tolua_export

	/** Moves the player to the specified world.
	Returns true if successful, false on failure (world not found). */
	bool MoveToWorld(const char * a_WorldName);  // tolua_export

	bool SaveToDisk(void);
	bool LoadFromDisk(void);
	void LoadPermissionsFromDisk(void);											// tolua_export

	const AString & GetLoadedWorldName() { return m_LoadedWorldName; }

	void UseEquippedItem(void);
	
	void SendHealth(void);

	void SendExperience(void);
	
	// In UI windows, the item that the player is dragging:
	bool IsDraggingItem(void) const { return !m_DraggingItem.IsEmpty(); }
	cItem & GetDraggingItem(void) {return m_DraggingItem; }
	
	// In UI windows, when inventory-painting:
	/** Clears the list of slots that are being inventory-painted. To be used by cWindow only */
	void ClearInventoryPaintSlots(void);
	
	/** Adds a slot to the list for inventory painting. To be used by cWindow only */
	void AddInventoryPaintSlot(int a_SlotNum);
	
	/** Returns the list of slots currently stored for inventory painting. To be used by cWindow only */
	const cSlotNums & GetInventoryPaintSlots(void) const;
	
	// tolua_begin
	
	/** Returns the current relative maximum speed (takes current sprinting / flying state into account) */
	double GetMaxSpeed(void) const;
	
	/** Gets the normal relative maximum speed */
	double GetNormalMaxSpeed(void) const { return m_NormalMaxSpeed; }
	
	/** Gets the sprinting relative maximum speed */
	double GetSprintingMaxSpeed(void) const { return m_SprintingMaxSpeed; }
	
	/** Gets the flying relative maximum speed */
	double GetFlyingMaxSpeed(void) const { return m_FlyingMaxSpeed; }
	
	/** Sets the normal relative maximum speed. Sends the update to player, if needed. */
	void SetNormalMaxSpeed(double a_Speed);
	
	/** Sets the sprinting relative maximum speed. Sends the update to player, if needed. */
	void SetSprintingMaxSpeed(double a_Speed);
	
	/** Sets the flying relative maximum speed. Sends the update to player, if needed. */
	void SetFlyingMaxSpeed(double a_Speed);
	
	/** Sets the crouch status, broadcasts to all visible players */
	void SetCrouch(bool a_IsCrouched);
	
	/** Starts or stops sprinting, sends the max speed update to the client, if needed */
	void SetSprint(bool a_IsSprinting);
	
	/** Flags the player as flying */
	void SetFlying(bool a_IsFlying);

	/** If true the player can fly even when he's not in creative. */
	void SetCanFly(bool a_CanFly);

	/** Update movement-related statistics. */
	void UpdateMovementStats(const Vector3d & a_DeltaPos);

	/** Returns wheter the player can fly or not. */
	virtual bool CanFly(void) const { return m_CanFly; }
	// tolua_end

	// cEntity overrides:
	virtual bool IsCrouched (void) const { return m_IsCrouched; }
	virtual bool IsSprinting(void) const { return m_IsSprinting; }
	virtual bool IsRclking  (void) const { return IsEating(); }

	virtual void Detach(void);
	
protected:
	typedef std::map< std::string, bool > PermissionMap;
	PermissionMap m_ResolvedPermissions;
	PermissionMap m_Permissions;

	GroupList m_ResolvedGroups;
	GroupList m_Groups;

	AString m_PlayerName;
	AString m_LoadedWorldName;

	/** Xp Level stuff */
	enum 
	{
		XP_TO_LEVEL15 = 255,
		XP_PER_LEVEL_TO15 = 17,
		XP_TO_LEVEL30 = 825
	} ;

	bool m_bVisible;

	// Food-related variables:
	/** Represents the food bar, one point equals half a "drumstick" */
	int m_FoodLevel;
	
	/** "Overcharge" for the m_FoodLevel; is depleted before m_FoodLevel */
	double m_FoodSaturationLevel;
	
	/** Count-up to the healing or damaging action, based on m_FoodLevel */
	int   m_FoodTickTimer;
	
	/** A "buffer" which adds up hunger before it is substracted from m_FoodSaturationLevel or m_FoodLevel. Each action adds a little */
	double m_FoodExhaustionLevel;
	
	/** Number of ticks remaining for the foodpoisoning effect; zero if not foodpoisoned */
	int m_FoodPoisonedTicksRemaining;
	
	float m_LastJumpHeight;
	float m_LastGroundHeight;
	bool m_bTouchGround;
	double m_Stance;
	cInventory m_Inventory;
	cWindow * m_CurrentWindow;
	cWindow * m_InventoryWindow;

	char m_Color;

	eGameMode m_GameMode;
	AString m_IP;
	
	/** The item being dragged by the cursor while in a UI window */
	cItem m_DraggingItem;

	long long m_LastPlayerListTime;
	static const unsigned short PLAYER_LIST_TIME_MS = 1000; // 1000 = once per second

	cClientHandle * m_ClientHandle;
	
	cSlotNums m_InventoryPaintSlots;
	
	/** Max speed, relative to the game default.
	1 means regular speed, 2 means twice as fast, 0.5 means half-speed.
	Default value is 1. */
	double m_NormalMaxSpeed;
	
	/** Max speed, relative to the game default max speed, when sprinting.
	1 means regular speed, 2 means twice as fast, 0.5 means half-speed.
	Default value is 1.3. */
	double m_SprintingMaxSpeed;
	
	/** Max speed, relative to the game default flying max speed, when flying.
	1 means regular speed, 2 means twice as fast, 0.5 means half-speed.
	Default value is 1. */
	double m_FlyingMaxSpeed;
	
	bool m_IsCrouched;
	bool m_IsSprinting;
	bool m_IsFlying;
	bool m_IsSwimming;
	bool m_IsSubmerged;
	bool m_IsFishing;

	bool m_CanFly;  // If this is true the player can fly. Even if he is not in creative.

	/** The world tick in which eating will be finished. -1 if not eating */
	Int64 m_EatingFinishTick;

	/** Player Xp level */
	short int m_LifetimeTotalXp;
	short int m_CurrentXp;

	// flag saying we need to send a xp update to client
	bool m_bDirtyExperience;

	bool m_IsChargingBow;
	int  m_BowCharge;

	int m_FloaterID;

	cTeam * m_Team;

	cStatManager m_Stats;



	void ResolvePermissions(void);
	void ResolveGroups(void);

	virtual void Destroyed(void);

	/** Filters out damage for creative mode/friendly fire */
	virtual bool DoTakeDamage(TakeDamageInfo & TDI) override;

	/** Stops players from burning in creative mode */
	virtual void TickBurning(cChunk & a_Chunk) override;
	
	/** Called in each tick to handle food-related processing */
	void HandleFood(void);

	/** Called in each tick if the player is fishing to make sure the floater dissapears when the player doesn't have a fishing rod as equipped item. */
	void HandleFloater(void);

	/** Tosses a list of items. */
	void TossItems(const cItems & a_Items);

	/** Adds food exhaustion based on the difference between Pos and LastPos, sprinting status and swimming (in water block) */
	void ApplyFoodExhaustionFromMovement();

	/** Flag representing whether the player is currently in a bed
	Set by a right click on unoccupied bed, unset by a time fast forward or teleport */
	bool m_bIsInBed;

} ; // tolua_export




