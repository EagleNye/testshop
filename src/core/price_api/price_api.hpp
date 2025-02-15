#pragma once

#ifndef PRICE_API_HPP_NEROSHOP
#define PRICE_API_HPP_NEROSHOP

#include "../../core/currency_converter.hpp"

#include <optional>

class PriceApi
{
public:
    virtual ~PriceApi() = default;

    virtual std::optional<double> price(neroshop::Currency from, neroshop::Currency to) const = 0;
};

#endif // PRICESOURCE_H
