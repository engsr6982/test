#include "plugin/MyPlugin.h"

#include <cstddef>
#include <memory>
#include <string>

#include "ll/api/memory/Hook.h"
#include "ll/api/plugin/NativePlugin.h"
#include "ll/api/plugin/RegisterHelper.h"


#include "mc/common/wrapper/SharedPtr.h"
#include "mc/deps/core/string/HashedString.h"
#include "mc/world/item/Item.h"
#include "mc/world/item/ItemStackBase.h"
#include "mc/world/item/registry/ItemRegistry.h"
#include "mc/world/item/registry/ItemStack.h"


namespace my_plugin {

std::shared_ptr<std::vector<std::string>> it;
int                                       ct = 0;

LL_AUTO_TYPE_INSTANCE_HOOK(
    TestHook,
    HookPriority::Highest,
    ::ItemRegistry,
    "?registerItem@ItemRegistry@@AEAAXV?$SharedPtr@VItem@@@@@Z",
    void,
    SharedPtr<Item>& a1
) {
    if (!it) {
        it = std::make_shared<std::vector<std::string>>();
    }
    ct++;
    origin(a1);
    if (a1.get() != nullptr) {
        try {
            auto& q1 = a1->getNamespace();
            if (!q1.empty()) it->push_back(q1);

            auto& q2 = a1->getFullNameHash();
            if (!q2.isEmpty()) {
                it->push_back(q2.str);
            }

            auto& q3 = a1->getRawNameHash();
            if (!q3.isEmpty()) {
                it->push_back(q3.str);
            }

            auto& q4 = a1->getRawNameId();
            if (!q4.empty()) it->push_back(q4);

            auto& q5 = a1->getFullItemName();
            if (!q5.empty()) it->push_back(q5);
        } catch (...) {}
    }
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

    for (auto const& i : *it) {
        logger.info(i);
    }
    logger.warn(std::to_string(it->size()));
    logger.error(std::to_string(ct));

    return true;
}

bool MyPlugin::disable() {
    getSelf().getLogger().info("Disabling...");
    // Code for disabling the plugin goes here.
    return true;
}

} // namespace my_plugin

LL_REGISTER_PLUGIN(my_plugin::MyPlugin, my_plugin::instance);
