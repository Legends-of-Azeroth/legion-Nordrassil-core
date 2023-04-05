#include "ScriptMgr.h"
#include "hellfire_citadel.h"
#include "InstanceScript.h"


class instance_hellfire_citadel : public InstanceMapScript
{
public:
    instance_hellfire_citadel() : InstanceMapScript("instance_hellfire_citadel", 1448) {}

    struct instance_hellfire_citadel_InstanceMapScript : public InstanceScript
    {
        instance_hellfire_citadel_InstanceMapScript(Map* map) : InstanceScript(map) {}

        WorldLocation loc_res_pla;

        void Initialize() override
        {
            SetBossNumber(MAX_ENCOUNTER);
        }

        void OnCreatureCreate(Creature* creature) override {}

        void OnGameObjectCreate(GameObject* gameObject) override {}

        void SetData(uint32 type, uint32 data) override {}

        uint32 GetData(uint32 type) const override
        {
            return 0;
        }

        ObjectGuid GetGuidData(uint32 type) const
        {
            return ObjectGuid::Empty;
        }

        WorldLocation* GetClosestGraveYard(float x, float y, float z) override
        {
            uint32 graveyardId = 5022;

            if (WorldSafeLocsEntry const* gy = sWorldSafeLocsStore.LookupEntry(graveyardId))
            {
                loc_res_pla.Relocate(gy->Loc.X, gy->Loc.Y, gy->Loc.Z);
                loc_res_pla.SetMapId(gy->MapID);
            }

            return &loc_res_pla;
        }
    };

    InstanceScript* GetInstanceScript(InstanceMap* map) const override
    {
        return new instance_hellfire_citadel_InstanceMapScript(map);
    }
};

void AddSC_instance_hellfire_citadel()
{
    new instance_hellfire_citadel();
}
