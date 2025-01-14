#include <vector>
#include <string>
#include <cstring>

#include "rascaline.h"
#include "catch.hpp"
#include "helpers.hpp"

static void check_block(
    eqs_tensormap_t* descriptor,
    size_t block_id,
    std::vector<int32_t> samples,
    std::vector<int32_t> properties,
    std::vector<double> values,
    std::vector<int32_t> gradient_samples,
    std::vector<double> gradients
);

TEST_CASE("calculator name") {
    SECTION("dummy_calculator") {
        const char* HYPERS_JSON = R"({
            "cutoff": 3.5,
            "delta": 25,
            "name": "bar"
        })";
        auto* calculator = rascal_calculator("dummy_calculator", HYPERS_JSON);
        REQUIRE(calculator != nullptr);

        char buffer[256] = {0};
        CHECK_SUCCESS(rascal_calculator_name(calculator, buffer, sizeof(buffer)));
        CHECK(buffer == std::string("dummy test calculator with cutoff: 3.5 - delta: 25 - name: bar"));

        rascal_calculator_free(calculator);
    }

    SECTION("long strings") {
        auto name = std::string(2048, 'b');
        auto HYPERS_JSON = R"({
            "cutoff": 3.5,
            "delta": 25,
            "name": ")" + name + "\"}";

        auto* calculator = rascal_calculator("dummy_calculator", HYPERS_JSON.c_str());
        REQUIRE(calculator != nullptr);

        char* buffer = new char[4096];
        auto status = rascal_calculator_name(calculator, buffer, 256);
        CHECK(status == RASCAL_BUFFER_SIZE_ERROR);

        CHECK_SUCCESS(rascal_calculator_name(calculator, buffer, 4096));
        std::string expected = "dummy test calculator with cutoff: 3.5 - delta: 25 - ";
        expected += "name: " + name;
        CHECK(buffer == expected);

        delete[] buffer;

        rascal_calculator_free(calculator);
    }
}

TEST_CASE("calculator parameters") {
    SECTION("dummy_calculator") {
        std::string HYPERS_JSON = R"({
            "cutoff": 3.5,
            "delta": 25,
            "name": "bar"
        })";
        auto* calculator = rascal_calculator("dummy_calculator", HYPERS_JSON.c_str());
        REQUIRE(calculator != nullptr);

        char buffer[256] = {0};
        CHECK_SUCCESS(rascal_calculator_parameters(calculator, buffer, sizeof(buffer)));
        CHECK(buffer == HYPERS_JSON);

        rascal_calculator_free(calculator);
    }

    SECTION("long strings") {
        auto name = std::string(2048, 'b');
        auto HYPERS_JSON = R"({
            "cutoff": 3.5,
            "delta": 25,
            "name": ")" + name + "\"}";

        auto* calculator = rascal_calculator("dummy_calculator", HYPERS_JSON.c_str());
        REQUIRE(calculator != nullptr);

        char* buffer = new char[4096];
        auto status = rascal_calculator_parameters(calculator, buffer, 256);
        CHECK(status == RASCAL_BUFFER_SIZE_ERROR);

        CHECK_SUCCESS(rascal_calculator_parameters(calculator, buffer, 4096));
        CHECK(buffer == HYPERS_JSON);

        delete[] buffer;

        rascal_calculator_free(calculator);
    }
}

TEST_CASE("calculator creation errors") {
    const char* HYPERS_JSON = R"({
        "cutoff": "532",
        "delta": 25,
        "name": "bar"
    })";
    auto *calculator = rascal_calculator("dummy_calculator", HYPERS_JSON);
    CHECK(calculator == nullptr);

    CHECK(std::string(rascal_last_error()) == "json error: invalid type: string \"532\", expected f64 at line 2 column 23");
}

TEST_CASE("Compute descriptor") {
    const char* HYPERS_JSON = R"({
        "cutoff": 3.0,
        "delta": 4,
        "name": ""
    })";

    SECTION("Full compute") {
        auto system = simple_system();

        rascal_calculation_options_t options = {0};
        const char* gradients_list[] = {"positions"};
        options.gradients = gradients_list;
        options.gradients_count = 1;
        auto* calculator = rascal_calculator("dummy_calculator", HYPERS_JSON);
        REQUIRE(calculator != nullptr);

        eqs_tensormap_t* descriptor = nullptr;
        auto status = rascal_calculator_compute(
            calculator, &descriptor, &system, 1, options
        );
        CHECK_SUCCESS(status);

        eqs_labels_t keys = {0};
        status = eqs_tensormap_keys(descriptor, &keys);
        CHECK_SUCCESS(status);

        CHECK(keys.size == 1);
        CHECK(keys.names[0] == std::string("species_center"));
        CHECK(keys.count == 2);
        CHECK(keys.values[0] == 1);
        CHECK(keys.values[1] == 6);

        auto samples = std::vector<int32_t>{
            0, 1, /**/ 0, 2, /**/ 0, 3,
        };
        auto properties = std::vector<int32_t>{
            1, 0, /**/ 0, 1,
        };
        auto values = std::vector<double>{
            5, 9, /**/ 6, 18, /**/ 7, 15,
        };
        auto gradient_samples = std::vector<int32_t>{
            0, 0, 0, /**/ 0, 0, 1, /**/ 0, 0, 2,
            1, 0, 1, /**/ 1, 0, 2, /**/ 1, 0, 3,
            2, 0, 2, /**/ 2, 0, 3,
        };
        auto gradients = std::vector<double>{
            0.0, 1.0, /**/ 0.0, 1.0, /**/ 0.0, 1.0,
            0.0, 1.0, /**/ 0.0, 1.0, /**/ 0.0, 1.0,
            0.0, 1.0, /**/ 0.0, 1.0, /**/ 0.0, 1.0,
            0.0, 1.0, /**/ 0.0, 1.0, /**/ 0.0, 1.0,
            0.0, 1.0, /**/ 0.0, 1.0, /**/ 0.0, 1.0,
            0.0, 1.0, /**/ 0.0, 1.0, /**/ 0.0, 1.0,
            0.0, 1.0, /**/ 0.0, 1.0, /**/ 0.0, 1.0,
            0.0, 1.0, /**/ 0.0, 1.0, /**/ 0.0, 1.0
        };

        // H block
        check_block(descriptor, 0, samples, properties, values, gradient_samples, gradients);

        samples = std::vector<int32_t>{
            0, 0,
        };
        values = std::vector<double>{
            4, 3,
        };
        gradient_samples = std::vector<int32_t>{
            0, 0, 0, /**/ 0, 0, 1,
        };
        gradients = std::vector<double>{
            0.0, 1.0, /**/ 0.0, 1.0, /**/ 0.0, 1.0,
            0.0, 1.0, /**/ 0.0, 1.0, /**/ 0.0, 1.0
        };

        // C block
        check_block(descriptor, 1, samples, properties, values, gradient_samples, gradients);

        eqs_tensormap_free(descriptor);
        rascal_calculator_free(calculator);
    }

    SECTION("Partial compute -- samples") {
        auto selected_samples_values = std::vector<int32_t>{
            0, 1, /**/ 0, 3,
        };
        auto selected_samples_names = std::vector<const char*>{
            "structure", "center"
        };

        eqs_labels_t selected_samples = {0};
        selected_samples.names = selected_samples_names.data();
        selected_samples.values = selected_samples_values.data();
        selected_samples.count = 2;
        selected_samples.size = 2;

        auto system = simple_system();

        rascal_calculation_options_t options = {0};
        const char* gradients_list[] = {"positions"};
        options.gradients = gradients_list;
        options.gradients_count = 1;
        options.selected_samples.subset = &selected_samples;
        auto* calculator = rascal_calculator("dummy_calculator", HYPERS_JSON);
        REQUIRE(calculator != nullptr);

        eqs_tensormap_t* descriptor = nullptr;
        auto status = rascal_calculator_compute(
            calculator, &descriptor, &system, 1, options
        );
        CHECK_SUCCESS(status);

        eqs_labels_t keys = {0};
        status = eqs_tensormap_keys(descriptor, &keys);
        CHECK_SUCCESS(status);

        CHECK(keys.size == 1);
        CHECK(keys.names[0] == std::string("species_center"));
        CHECK(keys.count == 2);
        CHECK(keys.values[0] == 1);
        CHECK(keys.values[1] == 6);

        auto samples = std::vector<int32_t>{
            0, 1, /**/ 0, 3,
        };
        auto properties = std::vector<int32_t>{
            1, 0, /**/ 0, 1,
        };
        auto values = std::vector<double>{
            5, 9, /**/ 7, 15,
        };
        auto gradient_samples = std::vector<int32_t>{
            0, 0, 0, /**/ 0, 0, 1, /**/ 0, 0, 2,
            1, 0, 2, /**/ 1, 0, 3,
        };
        auto gradients = std::vector<double>{
            0.0, 1.0, /**/ 0.0, 1.0, /**/ 0.0, 1.0,
            0.0, 1.0, /**/ 0.0, 1.0, /**/ 0.0, 1.0,
            0.0, 1.0, /**/ 0.0, 1.0, /**/ 0.0, 1.0,
            0.0, 1.0, /**/ 0.0, 1.0, /**/ 0.0, 1.0,
            0.0, 1.0, /**/ 0.0, 1.0, /**/ 0.0, 1.0
        };

        // H block
        check_block(descriptor, 0, samples, properties, values, gradient_samples, gradients);

        samples = std::vector<int32_t>{};
        values = std::vector<double>{};
        gradient_samples = std::vector<int32_t>{};
        gradients = std::vector<double>{};

        // C block
        check_block(descriptor, 1, samples, properties, values, gradient_samples, gradients);

        eqs_tensormap_free(descriptor);
        rascal_calculator_free(calculator);
    }

    SECTION("Partial compute -- features") {
        auto selected_properties_values = std::vector<int32_t>{
            0, 1,
        };
        auto selected_properties_names = std::vector<const char*>{
            "index_delta", "x_y_z"
        };

        eqs_labels_t selected_properties = {0};
        selected_properties.names = selected_properties_names.data();
        selected_properties.values = selected_properties_values.data();
        selected_properties.count = 1;
        selected_properties.size = 2;

        auto system = simple_system();

        rascal_calculation_options_t options = {0};
        const char* gradients_list[] = {"positions"};
        options.gradients = gradients_list;
        options.gradients_count = 1;
        options.selected_properties.subset = &selected_properties;
        auto* calculator = rascal_calculator("dummy_calculator", HYPERS_JSON);
        REQUIRE(calculator != nullptr);

        eqs_tensormap_t* descriptor = nullptr;
        auto status = rascal_calculator_compute(
            calculator, &descriptor, &system, 1, options
        );
        CHECK_SUCCESS(status);

        eqs_labels_t keys = {0};
        status = eqs_tensormap_keys(descriptor, &keys);
        CHECK_SUCCESS(status);

        CHECK(keys.size == 1);
        CHECK(keys.names[0] == std::string("species_center"));
        CHECK(keys.count == 2);
        CHECK(keys.values[0] == 1);
        CHECK(keys.values[1] == 6);

        auto samples = std::vector<int32_t>{
            0, 1, /**/ 0, 2, /**/ 0, 3,
        };
        auto properties = std::vector<int32_t>{
            0, 1,
        };
        auto values = std::vector<double>{
            9, /**/ 18, /**/ 15,
        };
        auto gradient_samples = std::vector<int32_t>{
            0, 0, 0, /**/ 0, 0, 1, /**/ 0, 0, 2,
            1, 0, 1, /**/ 1, 0, 2, /**/ 1, 0, 3,
            2, 0, 2, /**/ 2, 0, 3,
        };
        auto gradients = std::vector<double>{
            1.0, /**/ 1.0, /**/ 1.0,
            1.0, /**/ 1.0, /**/ 1.0,
            1.0, /**/ 1.0, /**/ 1.0,
            1.0, /**/ 1.0, /**/ 1.0,
            1.0, /**/ 1.0, /**/ 1.0,
            1.0, /**/ 1.0, /**/ 1.0,
            1.0, /**/ 1.0, /**/ 1.0,
            1.0, /**/ 1.0, /**/ 1.0
        };

        // H block
        check_block(descriptor, 0, samples, properties, values, gradient_samples, gradients);

        samples = std::vector<int32_t>{
            0, 0,
        };
        values = std::vector<double>{
            3,
        };
        gradient_samples = std::vector<int32_t>{
            0, 0, 0, /**/ 0, 0, 1,
        };
        gradients = std::vector<double>{
            1.0, /**/ 1.0, /**/ 1.0,
            1.0, /**/ 1.0, /**/ 1.0
        };

        // C block
        check_block(descriptor, 1, samples, properties, values, gradient_samples, gradients);

        eqs_tensormap_free(descriptor);
        rascal_calculator_free(calculator);
    }

    SECTION("Partial compute -- preselected") {
        auto samples_names = std::vector<const char*>{
            "structure", "center"
        };
        auto properties_names = std::vector<const char*>{
            "index_delta", "x_y_z"
        };

        auto h_samples_values = std::vector<int32_t>{
            0, 3,
        };
        auto h_properties_values = std::vector<int32_t>{
            0, 1,
        };

        eqs_block_t* blocks[2] = {nullptr, nullptr};

        eqs_labels_t h_samples = {0};
        h_samples.size = 2;
        h_samples.names = samples_names.data();
        h_samples.count = 1;
        h_samples.values = h_samples_values.data();

        eqs_labels_t h_properties = {0};
        h_properties.size = 2;
        h_properties.names = properties_names.data();
        h_properties.count = 1;
        h_properties.values = h_properties_values.data();
        blocks[0] = eqs_block(empty_array({1, 1}), h_samples, nullptr, 0, h_properties);
        REQUIRE(blocks[0] != nullptr);


        auto c_samples_values = std::vector<int32_t>{
            0, 0,
        };
        auto c_properties_values = std::vector<int32_t>{
            1, 0,
        };

        eqs_labels_t c_samples = {0};
        c_samples.size = 2;
        c_samples.names = samples_names.data();
        c_samples.count = 1;
        c_samples.values = c_samples_values.data();

        eqs_labels_t c_properties = {0};
        c_properties.size = 2;
        c_properties.names = properties_names.data();
        c_properties.count = 1;
        c_properties.values = c_properties_values.data();
        blocks[1] = eqs_block(empty_array({1, 1}), c_samples, nullptr, 0, c_properties);
        REQUIRE(blocks[1] != nullptr);

        auto keys_names = std::vector<const char*>{"species_center"};
        auto keys_values = std::vector<int32_t>{1, 6};

        eqs_labels_t keys = {0};
        keys.size = 1;
        keys.names = keys_names.data();
        keys.count = 2;
        keys.values = keys_values.data();

        auto predefined = eqs_tensormap(keys, blocks, 2);
        REQUIRE(predefined != nullptr);

        auto system = simple_system();
        rascal_calculation_options_t options = {0};
        const char* gradients_list[] = {"positions"};
        options.gradients = gradients_list;
        options.gradients_count = 1;
        options.selected_samples.predefined = predefined;
        options.selected_properties.predefined = predefined;
        auto* calculator = rascal_calculator("dummy_calculator", HYPERS_JSON);
        REQUIRE(calculator != nullptr);

        eqs_tensormap_t* descriptor = nullptr;
        auto status = rascal_calculator_compute(
            calculator, &descriptor, &system, 1, options
        );
        CHECK_SUCCESS(status);

        status = eqs_tensormap_keys(descriptor, &keys);
        CHECK_SUCCESS(status);

        CHECK(keys.size == 1);
        CHECK(keys.names[0] == std::string("species_center"));
        CHECK(keys.count == 2);
        CHECK(keys.values[0] == 1);
        CHECK(keys.values[1] == 6);

        auto samples = std::vector<int32_t>{
            0, 3,
        };
        auto properties = std::vector<int32_t>{
            0, 1,
        };
        auto values = std::vector<double>{
            15,
        };
        auto gradient_samples = std::vector<int32_t>{
            0, 0, 2, /**/ 0, 0, 3,
        };
        auto gradients = std::vector<double>{
            1.0, /**/ 1.0, /**/ 1.0,
            1.0, /**/ 1.0, /**/ 1.0
        };

        // H block
        check_block(descriptor, 0, samples, properties, values, gradient_samples, gradients);

        samples = std::vector<int32_t>{
            0, 0,
        };
        properties = std::vector<int32_t>{
            1, 0,
        };
        values = std::vector<double>{
            4,
        };
        gradient_samples = std::vector<int32_t>{
            0, 0, 0, /**/ 0, 0, 1,
        };
        gradients = std::vector<double>{
            0.0, /**/ 0.0, /**/ 0.0,
            0.0, /**/ 0.0, /**/ 0.0,
        };

        // C block
        check_block(descriptor, 1, samples, properties, values, gradient_samples, gradients);

        eqs_tensormap_free(predefined);
        eqs_tensormap_free(descriptor);
        rascal_calculator_free(calculator);
    }
}

void check_block(
    eqs_tensormap_t* descriptor,
    size_t block_id,
    std::vector<int32_t> samples,
    std::vector<int32_t> properties,
    std::vector<double> values,
    std::vector<int32_t> gradient_samples,
    std::vector<double> gradients
) {
    const eqs_block_t* block = nullptr;

    auto status = eqs_tensormap_block_by_id(descriptor, &block, block_id);
    CHECK_SUCCESS(status);

    /**************************************************************************/
    eqs_labels_t labels = {0};
    status = eqs_block_labels(block, "values", 0, &labels);
    CHECK_SUCCESS(status);

    CHECK(labels.size == 2);
    CHECK(labels.names[0] == std::string("structure"));
    CHECK(labels.names[1] == std::string("center"));
    auto n_samples = labels.count;

    auto label_values = std::vector<int32_t>(
        labels.values, labels.values + labels.count * labels.size
    );
    CHECK(label_values == samples);

    /**************************************************************************/
    status = eqs_block_labels(block, "values", 1, &labels);
    CHECK_SUCCESS(status);

    CHECK(labels.size == 2);
    CHECK(labels.names[0] == std::string("index_delta"));
    CHECK(labels.names[1] == std::string("x_y_z"));
    auto n_properties = labels.count;

    label_values = std::vector<int32_t>(
        labels.values, labels.values + labels.count * labels.size
    );
    CHECK(label_values == properties);

    /**************************************************************************/
    eqs_array_t data = {0};
    status = eqs_block_data(block, "values", &data);
    CHECK_SUCCESS(status);

    const double* values_ptr = nullptr;
    const uintptr_t* shape = nullptr;
    uintptr_t shape_count = 0;
    status = eqs_get_rust_array(&data, &values_ptr, &shape, &shape_count);
    CHECK_SUCCESS(status);

    CHECK(shape_count == 2);
    CHECK(shape[0] == n_samples);
    CHECK(shape[1] == n_properties);

    auto actual_values = std::vector<double>(
        values_ptr, values_ptr + n_samples * n_properties
    );
    CHECK(actual_values == values);

    /**************************************************************************/
    status = eqs_block_labels(block, "positions", 0, &labels);
    CHECK_SUCCESS(status);

    CHECK(labels.size == 3);
    CHECK(labels.names[0] == std::string("sample"));
    CHECK(labels.names[1] == std::string("structure"));
    CHECK(labels.names[2] == std::string("atom"));
    auto n_gradient_samples = labels.count;

    label_values = std::vector<int32_t>(
        labels.values, labels.values + labels.count * labels.size
    );
    CHECK(label_values == gradient_samples);

    /**************************************************************************/
    status = eqs_block_data(block, "positions", &data);
    CHECK_SUCCESS(status);

    status = eqs_get_rust_array(&data, &values_ptr, &shape, &shape_count);
    CHECK_SUCCESS(status);

    CHECK(shape_count == 3);
    CHECK(shape[0] == n_gradient_samples);
    CHECK(shape[1] == 3);
    CHECK(shape[2] == n_properties);

    auto actual_gradients = std::vector<double>(
        values_ptr, values_ptr + n_gradient_samples * 3 * n_properties
    );
    CHECK(actual_gradients == gradients);
}
