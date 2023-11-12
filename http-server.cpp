#include <httplib.h>
#include "enactor/Enactor.cpp"
#include <json.h>
#include <json-schema.h>
#include "component/CentralHub.hpp"
#include "component/centralHub.cpp"

static Enactor *enactor;

int get_adaptation_options(httplib::Request &req, httplib::Response &res)
{
    // Get the adaptation options from the Enactor class.
    const std::vector<std::string> adaptation_options = enactor->get_adaptation_options();

    // Convert the adaptation options to a JSON object.
    json json_adaptation_options;
    for (const std::string &adaptation_option : adaptation_options)
    {
        json_adaptation_options[adaptation_option] = adaptation_option;
    }
    // Return the JSON object in the HTTP response.
    res.set_status(200);
    res.set_header("Content-Type", "application/json");
    res.set_content(json_adaptation_options.dump());

    return 0;
}

private:
json_schema_t *execute_schema;
json_schema_t *adaptation_options_schema;

struct Enactor
{
    httplib::Server server;

    Enactor()
    {
        server.add_route("/execute", httplib::Method::PUT, execute_adaptation);
        server.add_route("/adaptation_options", httplib::Method::GET, get_adaptation_options);
        execute_schema = json_schema_new();

        server.add_route("/execute_schema", httplib::Method::GET, get_execute_schema);

        // Create the JSON schema of the JSON object returned by the /adaptation_options endpoint.
        adaptation_options_schema = json_schema_new();

        json_schema_t *Enactor::get_execute_schema() const
        {
            // Create the JSON schema of the JSON object returned by the /execute endpoint.
            json_schema_t *execute_schema = json_schema_new();
            json_schema_t *adaptation_schema = json_schema_new();
            adaptation_schema->type = json_schema_type_string;

            json_schema_add_property(execute_schema, "adaptation", adaptation_schema);
            json_schema_t *component_schema = json_schema_new();
            component_schema->type = json_schema_type_string;

            json_schema_add_property(execute_schema, "component", component_schema);
            json_schema_t *frequency_schema = json_schema_new();
            frequency_schema->type = json_schema_type_number;

            json_schema_add_property(execute_schema, "frequency", frequency_schema);

            return execute_schema;
        }
        // Add the JSON schema to the HTTP server.
        server.add_route("/adaptation_options_schema", httplib::Method::GET, get_adaptation_options_schema);
        server.add_route("/execute_schema", httplib::Method::GET, get_execute_schema);
        server.add_route("/adaptation_options_schema", httplib::Method::GET, get_adaptation_options_schema);

    public:
        json_schema_t *get_execute_schema() const
        {
            return execute_schema;
        }
        json_schema_t *get_adaptation_options_schema() const
        {
            return adaptation_options_schema;
        }
    }

    void execute_adaptation(httplib::Request &req, httplib::Response &res)
    {
        // JSON object in the request body.
        json_object *json = json_tokener_parse(req.body);
        // Get the adaptation to enact.
        const char *adaptation = json_object_get_string(json, "adaptation");
        enact_adaptation(adaptation);

        // Return a JSON response with the status of the adaptation.
        json_object *res_json = json_object_new_object();
        json_object_set_string(res_json, "status", "success");
        res.set_status(200);
        res.set_header("Content-Type", "application/json");
        res.set_content(json_object_to_json_string(res_json));
    }

    void enact_adaptation(const char *adaptation)
    {
        if (strcmp(adaptation, "change_frequency") == 0)
        {
            // Get the component name and frequency from the JSON object.
            const char *component = json_object_get_string(req.body, "component");
            double frequency = json_object_get_number(req.body, "frequency");
        }

        /*
        example json for execute endpoint
        {
          "adaptation": "change_frequency",
          "component": "g4t1",
          "frequency": 10.0
        }
        */
    };

}

class CentralHub
{

public:
    CentralHub(int &argc, char **argv, const std::string &name, const bool &active, const bsn::resource::Battery &battery) : Component(argc, argv, name), active(active), max_size(20), total_buffer_size(0), buffer_size({0, 0, 0, 0, 0, 0}), battery(battery)
    {
        this->http_server = new httplib::Server("0.0.0.0", 7070);
        // endpoint for monitoring data.
        this->http_server->AddRoute("/monitor", httplib::Method::GET, [](const httplib::Request &req, httplib::Response &res)
                                    {
        server.add_route("/monitor_schema", httplib::Method::GET, get_monitor_schema);
          std::vector<std::string> data = this->monitorAllData();
          res.SetContentType("application/json");
          res.SetBody(json::serialize(data)); });
    }

    ~CentralHub()
    {
        delete this->http_server;
    }

    void body() override
    {
        this->http_server->Start();
    }

private:
    httplib::Server *http_server;
    json_schema_t *monitor_schema;
    monitor_schema = json_schema_new();

    void CentralHub::create_monitor_schema()
    {
        // JSON schema for the monitor endpoint response.
        json_schema_t *monitor_schema = json_schema_new();

        // properties of component in the system.
        for (const Component &component : components)
        {
            json_schema_t *component_schema = json_schema_new();

            // Add metric of each  monitored component.
            for (const Metric &metric : component.metrics)
            {
                json_schema_t *metric_schema = json_schema_new();
                metric_schema->type = json_schema_type_number;
                json_schema_add_property(component_schema, metric.name, metric_schema);
                json_schema_free(metric_schema);
            }

            // Add the component schema to monitor endpoint response schema.
            json_schema_add_property(monitor_schema, component.name, component_schema);
            json_schema_free(component_schema);
        }

        // monitor endpoint response schema.
        this->monitor_schema = monitor_schema;
    }
};

int main(int argc, char *argv[])
{
    CentralHub central_hub(argc, argv, "CentralHub", true, bsn::resource::Battery());
    central_hub.Start();
}

~CentralHub()
{
    server.stop();
    json_schema_free(monitor_schema);
}

void get_monitor_schema(httplib::Request &req, httplib::Response &res)
{
    res.set_header("Content-Type", "application/json");
    res.set_content(json_schema_dump(monitor_schema));
}
CentralHub::~CentralHub()
{
    delete http_server;
}

int main()
{
    enactor = new Enactor();
    httplib::Server server("localhost", 7070);
    // Add a route for the `/adaptation_options endpoint.
    server.add_route("/adaptation_options", httplib::Method::GET, get_adaptation_options);

    server.start();
    server.wait();
    delete enactor;

    return 0;
}
