var scopes = require('unity-js-scopes')
@if "%ContentType%" != "empty"
var http = require('http');
@if "%ContentType%" == "http+xml"
var XML = require('pixl-xml');
@endif

var query_host = "api.openweathermap.org"
@if "%ContentType%" == "http+xml"
var current_weather_path = "/data/2.5/weather?units=metric&APPID=2b12bf09b4e0ab0c1aa5e32a9a3f0cdc&mode=xml&q="
var forecast_weather_path = "/data/2.5/forecast/daily/?units=metric&cnt=7&APPID=2b12bf09b4e0ab0c1aa5e32a9a3f0cdc&mode=xml&q="
@elsif "%ContentType%" == "http+json"
var current_weather_path = "/data/2.5/weather?units=metric&APPID=2b12bf09b4e0ab0c1aa5e32a9a3f0cdc&q="
var forecast_weather_path = "/data/2.5/forecast/daily/?units=metric&cnt=7&APPID=2b12bf09b4e0ab0c1aa5e32a9a3f0cdc&q="
@endif

var CURRENT_TEMPLATE =
{
    "schema-version": 1,
    "template": {
        "category-layout": "grid",
        "card-size": "medium"
    },
    "components": {
        "title": "title",
        "art": {
            "field": "art"
        },
        "subtitle": "subtitle"
    }
}

var FORECAST_TEMPLATE =
{
    "schema-version": 1,
    "template": {
        "category-layout": "grid",
        "card-layout": "horizontal",
        "card-size": "small"
    },
    "components": {
        "title": "title",
        "art" : {
            "field": "art"
        },
        "subtitle": "subtitle"
    }
}
@endif

scopes.self.initialize(
            {}
            ,
            {
                run: function() {
                    console.log('Running...')
                },
                start: function(scope_id) {
                    console.log('Starting scope id: '
                                + scope_id
                                + ', '
                                + scopes.self.scope_config)
                },
                search: function(canned_query, metadata) {
                    return new scopes.lib.search_query(
                                canned_query,
                                metadata,
                                // run
                                function(search_reply) {
@if "%ContentType%" != "empty"
                                    var qs = canned_query.query_string();
                                    if (!qs) {
                                        qs = "London,uk"
                                    }

                                    var current_weather_cb = function(response) {
                                        var res = '';

                                        // Another chunk of data has been recieved, so append it to res
                                        response.on('data', function(chunk) {
                                            res += chunk;
                                        });

                                        // The whole response has been recieved
                                        response.on('end', function() {
@if "%ContentType%" == "http+xml"
                                            r = XML.parse(res);

                                            var category_renderer = new scopes.lib.category_renderer(JSON.stringify(CURRENT_TEMPLATE));
                                            var category = search_reply.register_category("current", r.city.name + ", " + r.city.country, "", category_renderer);

                                            var categorised_result = new scopes.lib.categorised_result(category);
                                            categorised_result.set_uri(r.city.id.toString());
                                            categorised_result.set_title(r.temperature.value.toString() + "°C");
                                            categorised_result.set_art("http://openweathermap.org/img/w/" + r.weather.icon + ".png");
                                            categorised_result.set("subtitle", r.weather.value);
                                            categorised_result.set("description", "A description of the result");
@elsif "%ContentType%" == "http+json"
                                            r = JSON.parse(res);

                                            var category_renderer = new scopes.lib.category_renderer(JSON.stringify(CURRENT_TEMPLATE));
                                            var category = search_reply.register_category("current", r.name + ", " + r.sys.country, "", category_renderer);

                                            var categorised_result = new scopes.lib.categorised_result(category);
                                            categorised_result.set_uri(r.id.toString());
                                            categorised_result.set_title(r.main.temp.toString() + "°C");
                                            categorised_result.set_art("http://openweathermap.org/img/w/" + r.weather[0].icon + ".png");
                                            categorised_result.set("subtitle", r.weather[0].description);
                                            categorised_result.set("description", "A description of the result");
@endif

                                            search_reply.push(categorised_result);

                                            // Now call back into the API for a 7 day forecast
                                            http.request({host: query_host, path: forecast_weather_path + qs}, forecase_weather_cb).end();
                                        });
                                    }

                                    var forecase_weather_cb = function(response) {
                                        var res = '';

                                        // Another chunk of data has been recieved, so append it to res
                                        response.on('data', function(chunk) {
                                            res += chunk;
                                        });

                                        // The whole response has been recieved
                                        response.on('end', function() {
                                            try {
@if "%ContentType%" == "http+xml"
                                                r = XML.parse(res);

                                                var category_renderer = new scopes.lib.category_renderer(JSON.stringify(FORECAST_TEMPLATE));
                                                var category = search_reply.register_category("forecast", "7 day forecast", "", category_renderer);

                                                var weather_id = 1000000;
                                                for (i = 0; i < r.forecast.time.length; i++) {
                                                    var categorised_result = new scopes.lib.categorised_result(category);
                                                    categorised_result.set_uri((weather_id++).toString());
                                                    categorised_result.set_title(r.forecast.time[i].temperature.min.toString() + "°C to "
                                                                                 + r.forecast.time[i].temperature.max.toString() + "°C");
                                                    categorised_result.set_art("http://openweathermap.org/img/w/" + r.forecast.time[i].symbol.var + ".png");
                                                    categorised_result.set("subtitle", r.forecast.time[i].symbol.name);
                                                    categorised_result.set("description", "A description of the result");

                                                    search_reply.push(categorised_result);
                                                }
@elsif "%ContentType%" == "http+json"
                                                r = JSON.parse(res);

                                                var category_renderer = new scopes.lib.category_renderer(JSON.stringify(FORECAST_TEMPLATE));
                                                var category = search_reply.register_category("forecast", "7 day forecast", "", category_renderer);

                                                for (i = 0; i < r.list.length; i++) {
                                                    var categorised_result = new scopes.lib.categorised_result(category);
                                                    categorised_result.set_uri(r.list[i].weather[0].id.toString());
                                                    categorised_result.set_title(r.list[i].temp.min.toString() + "°C to "
                                                                                 + r.list[i].temp.max.toString() + "°C");
                                                    categorised_result.set_art("http://openweathermap.org/img/w/" + r.list[i].weather[0].icon + ".png");
                                                    categorised_result.set("subtitle", r.list[i].weather[0].description);
                                                    categorised_result.set("description", "A description of the result");

                                                    search_reply.push(categorised_result);
                                                }
@endif

                                                // We are done, call finished() on our search_reply
                                                search_reply.finished();
                                            }
                                            catch(e) {
                                                // Forecast not available
                                                console.log("Forecast for '" + qs + "' is unavailable: " + e)
                                            }
                                        });
                                    }

                                    http.request({host: query_host, path: current_weather_path + qs}, current_weather_cb).end();
@endif
                                },
                                // cancelled
                                function() {
                                });
                },
                preview: function(result, action_metadata) {
                    return new scopes.lib.preview_query(
                                result,
                                action_metadata,
                                // run
                                function(preview_reply) {
@if "%ContentType%" != "empty"
                                    var layout1col = new scopes.lib.column_layout(1);
                                    var layout2col = new scopes.lib.column_layout(2);
                                    var layout3col = new scopes.lib.column_layout(3);
                                    layout1col.add_column(["image", "header", "summary"]);

                                    layout2col.add_column(["image"]);
                                    layout2col.add_column(["header", "summary"]);

                                    layout3col.add_column(["image"]);
                                    layout3col.add_column(["header", "summary"]);
                                    layout3col.add_column([]);

                                    preview_reply.register_layout([layout1col, layout2col, layout3col]);

                                    var header = new scopes.lib.preview_widget("header", "header");
                                    header.add_attribute_mapping("title", "title");
                                    header.add_attribute_mapping("subtitle", "subtitle");

                                    var image = new scopes.lib.preview_widget("image", "image");
                                    image.add_attribute_mapping("source", "art");

                                    var description = new scopes.lib.preview_widget("summary", "text");
                                    description.add_attribute_mapping("text", "description");

                                    preview_reply.push([image, header, description ]);
                                    preview_reply.finished();
@endif
                                },
                                // cancelled
                                function() {
                                });
                }
            }
            );
