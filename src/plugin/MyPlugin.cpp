#include "plugin/MyPlugin.h"

#include <cstddef>
#include <memory>
#include <string>

#include "ll/api/Expected.h"
#include "ll/api/memory/Hook.h"
#include "ll/api/plugin/NativePlugin.h"
#include "ll/api/plugin/RegisterHelper.h"


#include "ll/api/command/Command.h"
#include "ll/api/command/CommandHandle.h"
#include "ll/api/command/CommandRegistrar.h"
#include "ll/api/command/Overload.h"
#include "ll/api/command/OverloadData.h"
#include "ll/api/service/Bedrock.h"
#include "ll/api/service/Service.h"
#include "ll/api/utils/StringUtils.h"
#include "mc/common/wrapper/SharedPtr.h"
#include "mc/deps/core/string/HashedString.h"
#include "mc/enums/d_b_helpers/Category.h"
#include "mc/nbt/CompoundTag.h"
#include "mc/server/commands/CommandOrigin.h"
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


template <typename T>
inline T* Global = nullptr;

namespace my_plugin {


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
MyPlugin&                        MyPlugin::getInstance() { return *instance; }
bool                             MyPlugin::load() {
    getSelf().getLogger().info("Loading...");
    return true;
}

using string = std::string;
struct PString {
    string dim;
};

bool MyPlugin::enable() {
    getSelf().getLogger().info("Enabling...");

    auto& cmd = ll::command::CommandRegistrar::getInstance().getOrCreateCommand("db");

    cmd.overload().text("li").execute([](CommandOrigin const&, CommandOutput&) {
        (*Global<DBStorage>)
            .forEachKeyWithPrefix("", DBHelpers::Category::All, [](std::string_view a1, std::string_view) {
                std::cout << a1 << std::endl;
            });
    });
    cmd.overload<PString>().text("del").required("dim").execute(
        [](CommandOrigin const&, CommandOutput&, PString const& a1) {
            (*Global<DBStorage>).deleteData(a1.dim.c_str(), DBHelpers::Category::All);
        }
    );
    cmd.overload<PString>().text("has").required("dim").execute(
        [](CommandOrigin const&, CommandOutput&, PString const& a1) {
            auto res = (*Global<DBStorage>).hasKey(a1.dim.c_str(), DBHelpers::Category::All);
            std::cout << (res ? "true" : "false") << std::endl;
        }
    );
    cmd.overload().text("tr").execute([](CommandOrigin const&, CommandOutput&) {
        auto& res = (*Global<DBStorage>);

        string p;

        res.loadData("plot", p, DBHelpers::Category::All);

        res.saveData("Overworld", string(p), DBHelpers::Category::All);
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
