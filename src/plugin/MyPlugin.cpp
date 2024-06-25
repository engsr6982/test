#include "plugin/MyPlugin.h"

#include <cstddef>
#include <memory>
#include <string>

#include "ll/api/memory/Hook.h"
#include "ll/api/plugin/NativePlugin.h"
#include "ll/api/plugin/RegisterHelper.h"


#include "ll/api/service/Bedrock.h"
#include "ll/api/service/Service.h"
#include "mc/common/wrapper/SharedPtr.h"
#include "mc/deps/core/string/HashedString.h"
#include "mc/nbt/CompoundTag.h"
#include "mc/world/item/Item.h"
#include "mc/world/item/ItemStackBase.h"
#include "mc/world/item/registry/ItemRegistry.h"
#include "mc/world/item/registry/ItemStack.h"
#include "mc/world/level/Level.h"
#include <mc/server/commands/MinecraftCommands.h>
#include <mc/util/Random.h>
#include <mc/world/attribute/AttributeInstance.h>
#include <mc/world/attribute/SharedAttributes.h>
#include <mc/world/effect/MobEffect.h>
#include <mc/world/effect/MobEffectInstance.h>
#include <mc/world/level/dimension/VanillaDimensions.h>
#include <mc/world/level/storage/DBStorage.h>


namespace my_plugin {

template <typename T>
inline T* Global = nullptr;

LL_AUTO_TYPE_INSTANCE_HOOK(
    DBStorageInitEvent,
    ll::memory::HookPriority::Normal,
    DBStorage,
    "??0DBStorage@@QEAA@UDBStorageConfig@@V?$not_null@V?$NonOwnerPointer@VLevelDbEnv@@@Bedrock@@@gsl@@@Z",
    DBStorage*,
    struct DBStorageConfig&                        a1,
    Bedrock::NotNullNonOwnerPtr<class LevelDbEnv>& a2
) {
    auto res          = origin(a1, a2);
    Global<DBStorage> = this;
    return res;
}

static std::unique_ptr<MyPlugin> instance;

MyPlugin& MyPlugin::getInstance() { return *instance; }

bool MyPlugin::load() {
    getSelf().getLogger().info("Loading...");
    // Code for loading the plugin goes here.
    return true;
}

bool MyPlugin::enable() {
    getSelf().getLogger().info("Enabling...");
    // Code for enabling the plugin goes here.

    auto& logger = getSelf().getLogger();
    auto& db     = *Global<DBStorage>;
    db.forEachKeyWithPrefix("_", ::DBHelpers::Category::All, [&logger](std::string_view a1, std::string_view a2) {
        logger.info("{}: {}", a1, a2);
    });

    return true;
}

bool MyPlugin::disable() {
    getSelf().getLogger().info("Disabling...");
    // Code for disabling the plugin goes here.
    return true;
}

} // namespace my_plugin

LL_REGISTER_PLUGIN(my_plugin::MyPlugin, my_plugin::instance);
