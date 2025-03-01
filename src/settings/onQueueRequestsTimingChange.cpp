#include "../queue/queueRequests.hpp"

#include <Geode/Geode.hpp>

using namespace geode::prelude;

$execute {
    listenForSettingChanges("queue-requests-timing", [](double value) {
        QueueRequests::get()->stopLoop();
    });
}