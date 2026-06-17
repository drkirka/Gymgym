#include "MeasurementView.h"

#include <iomanip>
#include <sstream>
#include <exception>

#include <nlohmann/json.hpp>

using namespace ftxui;

namespace {
std::string format_decimal(double value, int precision = 1) {
    std::ostringstream out;
    out << std::fixed << std::setprecision(precision) << value;
    std::string text = out.str();

    while (text.size() > 1 && text.back() == '0') {
        text.pop_back();
    }

    if (!text.empty() && text.back() == '.') {
        text.pop_back();
    }

    return text;
}

Element label_value(const std::string& label, const std::string& value) {
    return hbox({
        text(label + ": ") | color(Color::Yellow),
        text(value)
    });
}
}  // namespace

std::vector<MeasurementDto> parse_measurements_response(
    const std::string& response,
    std::string& error
) {
    std::vector<MeasurementDto> measurements;
    error.clear();

    try {
        auto parsed = nlohmann::json::parse(response);

        if (!parsed.contains("status") || parsed.value("status", "") != "OK") {
            error = parsed.value("message", "Server returned an error while loading measurements.");
            return measurements;
        }

        if (!parsed.contains("measurements") || !parsed["measurements"].is_array()) {
            error = "Server response did not contain a measurements list.";
            return measurements;
        }

        for (const auto& item : parsed["measurements"]) {
            MeasurementDto measurement;
            measurement.id = item.value("id", 0ULL);
            measurement.weight = item.value("weight", 0.0);
            measurement.body_fat = item.value("body_fat", 0.0);
            measurement.chest = item.value("chest", 0.0);
            measurement.waist = item.value("waist", 0.0);
            measurement.arm = item.value("arm", 0.0);
            measurement.leg = item.value("leg", 0.0);
            measurement.measured_at = item.value("measured_at", "");
            measurements.push_back(measurement);
        }
    }
    catch (const std::exception& ex) {
        error = std::string("Could not parse measurements response: ") + ex.what();
    }

    return measurements;
}

Element render_measurement_card(const MeasurementDto& measurement, std::size_t index) {
    return vbox({
        hbox({
            text("Measurement #" + std::to_string(index + 1)) | bold | color(Color::Cyan),
            text("  ID: " + std::to_string(measurement.id)) | color(Color::GrayLight)
        }),
        measurement.measured_at.empty()
            ? text("")
            : label_value("Measured at", measurement.measured_at),
        separator(),
        hbox({
            vbox({
                label_value("Weight", format_decimal(measurement.weight) + " kg"),
                label_value("Body fat", format_decimal(measurement.body_fat) + " %"),
            }) | flex,
            separator(),
            vbox({
                label_value("Chest", format_decimal(measurement.chest) + " cm"),
                label_value("Waist", format_decimal(measurement.waist) + " cm"),
            }) | flex,
            separator(),
            vbox({
                label_value("Arm", format_decimal(measurement.arm) + " cm"),
                label_value("Leg", format_decimal(measurement.leg) + " cm"),
            }) | flex,
        })
    }) | border;
}
