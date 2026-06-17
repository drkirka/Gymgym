#include "PersonalRecordView.h"

#include <exception>
#include <iomanip>
#include <sstream>

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

std::vector<PersonalRecordDto> parse_personal_records_response(
    const std::string& response,
    std::string& error
) {
    std::vector<PersonalRecordDto> records;
    error.clear();

    try {
        auto parsed = nlohmann::json::parse(response);

        if (!parsed.contains("status") || parsed.value("status", "") != "OK") {
            error = parsed.value("message", "Server returned an error while loading personal records.");
            return records;
        }

        if (!parsed.contains("records") || !parsed["records"].is_array()) {
            error = "Server response did not contain a personal records list.";
            return records;
        }

        for (const auto& item : parsed["records"]) {
            PersonalRecordDto record;
            record.id = item.value("id", 0ULL);
            record.exercise_id = item.value("exercise_id", 0ULL);
            record.exercise_name = item.value("exercise", item.value("exercise_name", std::string{}));
            record.weight = item.value("weight", item.value("weight_kg", 0.0));
            record.repetitions = item.value("repetitions", 0);
            record.achieved_at = item.value("achieved_at", "");
            records.push_back(record);
        }
    }
    catch (const std::exception& ex) {
        error = std::string("Could not parse personal records response: ") + ex.what();
    }

    return records;
}

Element render_personal_record_card(const PersonalRecordDto& record, std::size_t index) {
    std::string exercise = record.exercise_name.empty() ? "Unknown exercise" : record.exercise_name;

    return vbox({
        hbox({
            text("Record #" + std::to_string(index + 1)) | bold | color(Color::Cyan),
            text("  ID: " + std::to_string(record.id)) | color(Color::GrayLight)
        }),
        label_value("Exercise", exercise),
        record.exercise_id == 0
            ? text("")
            : label_value("Exercise ID", std::to_string(record.exercise_id)),
        record.achieved_at.empty()
            ? text("")
            : label_value("Achieved at", record.achieved_at),
        separator(),
        hbox({
            label_value("Weight", format_decimal(record.weight) + " kg") | flex,
            separator(),
            label_value("Repetitions", std::to_string(record.repetitions)) | flex,
        })
    }) | border;
}
