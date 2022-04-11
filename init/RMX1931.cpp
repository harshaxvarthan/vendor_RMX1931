/*
 * Copyright (c) 2021, The LineageOS Project
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <cstdlib>
#include <fstream>
#include <string.h>
#include <unistd.h>
#include <vector>

#include <android-base/properties.h>
#define _REALLY_INCLUDE_SYS__SYSTEM_PROPERTIES_H_
#include <sys/_system_properties.h>
#include <sys/sysinfo.h>

#include "property_service.h"
#include "vendor_init.h"

std::vector<std::string> ro_props_default_source_order = {
    "",
    "odm.",
    "product.",
    "system.",
    "system_ext.",
    "vendor.",
};

bool isCN()
{
    // Get project number
    std::ifstream infile("/proc/oppoVersion/prjName");
    std::string prj;
    bool ret = false;
    getline(infile, prj);
    if (prj.compare("19781"))
        ret = true;
    return ret;
}

void property_override(char const prop[], char const value[], bool add = true) {
    prop_info *pi;

    pi = (prop_info *)__system_property_find(prop);
    if (pi)
        __system_property_update(pi, value, strlen(value));
    else if (add)
        __system_property_add(prop, strlen(prop), value, strlen(value));
}

void set_device_props(const std::string fingerprint, const std::string description,
     const std::string brand, const std::string device, const std::string model, const std::string name)
{
     const auto set_ro_build_prop = [](const std::string &source,
                                      const std::string &prop,
                                      const std::string &value) {
        auto prop_name = "ro." + source + "build." + prop;
        property_override(prop_name.c_str(), value.c_str(), false);
    };

    const auto set_ro_product_prop = [](const std::string &source,
                                        const std::string &prop,
                                        const std::string &value) {
        auto prop_name = "ro.product." + source + prop;
        property_override(prop_name.c_str(), value.c_str(), false);
    };

    for (const auto &source : ro_props_default_source_order) {
        set_ro_build_prop(source, "fingerprint", fingerprint);
        set_ro_product_prop(source, "brand", brand);
        set_ro_product_prop(source, "device", device);
        set_ro_product_prop(source, "model", model);
        set_ro_product_prop(source, "name", name);
    }

    property_override("ro.build.fingerprint", fingerprint.c_str());
    property_override("ro.build.description", description.c_str());
    property_override("ro.system_ext.build.fingerprint", fingerprint.c_str());
}

void vendor_load_properties() {
    char const fp[] = "realme/RMX1931/RMX1931L1:10/QKQ1.191021.002/1583422340:user/release-keys";
    char const fp_cn[] = "realme/RMX1931/RMX1931CN:10/QKQ1.191021.002/1583422340:user/release-keys";
    char const fp_desc[] = "msmnile-user 10 QKQ1.191021.002 1583422340 release-keys";

    if (isCN()) {
        set_device_props(
            fp_cn,
            fp_desc,
            "Realme", "RMX1931CN", "Realme X2 Pro", "RMX1931");
    } else {
        set_device_props(
            fp,
            fp_desc,
            "Realme", "RMX1931L1", "Realme X2 Pro", "RMX1931");
    }
}
