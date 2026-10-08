#include "nativeroot/probes/permission_boundary_probe.h"
#include "nativeroot/probes/permission_boundary_netlink.h"
#include <android/api-level.h>
#include <iostream>

using namespace duckdetector::nativeroot;
using permission_boundary::check_netlink_link_boundary;
using permission_boundary::check_netlink_neigh_boundary;

int main(int argc, char* argv[]) {
	const int api_level = android_get_device_api_level();
	const int target_sdk = android_get_application_target_sdk_version();

	ProbeResult result;
	check_netlink_link_boundary(result, api_level, target_sdk);
	check_netlink_neigh_boundary(result, api_level, target_sdk);
	
    if (result.findings.empty() && result.checked_count == 0 && result.aux_flags == 0) return 1;

    std::cout << "   Duck Detect Results";
    std::cout << "\n-------------------------";
    std::cout << "\nAPI: " << api_level << " / TARGET: " << target_sdk;
    std::cout << "\n-------------------------";
    std::cout << "\n    Checked Count: " << result.checked_count;
    std::cout << "\n     Denied Count: " << result.denied_count;
    std::cout << "\n        Hit Count: " << result.hit_count << "\n";

    std::cout << "\n      Numeric Val: " << result.numeric_value;
    std::cout << "\nExtra Numeric Val: " << result.extra_numeric_value;
    std::cout << "\n        Aux Flags: " << result.aux_flags << "\n";

    std::cout << "\n-------------------------";
    std::cout << "\nExtra Text:\n" << result.extra_text;
    std::cout << "\nFindings:\n";

    for (const auto& finding : result.findings) {
        std::string sev_str;
        switch (finding.severity) {
            case Severity::kInfo:    sev_str = "INFO";    break;
            case Severity::kDanger:  sev_str = "DANGER";  break;
            case Severity::kWarning: sev_str = "WARNING"; break;
            default:                 sev_str = "UNKNOWN"; break;
        }

        std::cout << "  [" << sev_str << "] Group: " << finding.group << " | " << finding.label << " = " << finding.value << "\n";
        std::cout << "  Detail: " << finding.detail << "\n\n";
    }
    std::cout << "-------------------------\n";
    return 0;
}