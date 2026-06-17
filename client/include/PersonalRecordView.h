#pragma once

#include <cstddef>
#include <string>
#include <vector>

#include <ftxui/dom/elements.hpp>

#include "PersonalRecordDto.h"

std::vector<PersonalRecordDto> parse_personal_records_response(
    const std::string& response,
    std::string& error
);

ftxui::Element render_personal_record_card(
    const PersonalRecordDto& record,
    std::size_t index
);
