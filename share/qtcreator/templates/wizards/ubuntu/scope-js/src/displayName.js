var scopes = require('unity-js-scopes')
var http = require('http');

var query_host = "www.colourlovers.com"
var query_path = "/api/palettes?format=json&numResults=100&keywords="

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
            return scopes.lib.new_search_query(
                canned_query,
                metadata,
                // run
                function(search_reply) {
                    var qs = canned_query.query_string();
                    var cat_title = "";
                    if (!qs) {
                        cat_title = "Best palettes"
                    }
                    var category = search_reply.register_category("simple", cat_title, "");

                    var options = {
                        host: query_host,
                        path: query_path + qs
                    };

                    callback = function(response) {
                        var res = '';

                        // Another chunk of data has been recieved, so append it to res
                        response.on('data', function(chunk) {
                            res += chunk;
                        });

                        // The whole response has been recieved
                        response.on('end', function() {
                            r = JSON.parse(res);
                            for (i = 0; i < r.length; i++) {
                                var categorised_result = scopes.lib.new_categorised_result(category);
                                categorised_result.set_uri(r[i].url);
                                categorised_result.set_title(r[i].title);
                                categorised_result.set_art(r[i].imageUrl);
                                search_reply.push(categorised_result);

                                // Limit result count to 15
                                if (i == 14) {
                                    break;
                                }
                            }
                            search_reply.finished();
                        });
                    }

                    http.request(options, callback).end();
                },
                // cancelled
                function() {
                });
        },
        preview: function(result, action_metadata) {
            return scopes.lib.new_preview_query(
                result,
                action_metadata,
                // run
                function(preview_reply) {
                    var layout1col = scopes.lib.new_column_layout(1);
                    var layout2col = scopes.lib.new_column_layout(2);
                    var layout3col = scopes.lib.new_column_layout(3);
                    layout1col.add_column(["image", "header", "summary"]);

                    layout2col.add_column(["image"]);
                    layout2col.add_column(["header", "summary"]);

                    layout3col.add_column(["image"]);
                    layout3col.add_column(["header", "summary"]);
                    layout3col.add_column([]);

                    preview_reply.register_layout([layout1col, layout2col, layout3col]);

                    var header = scopes.lib.new_preview_widget("header", "header");
                    header.add_attribute_mapping("title", "title");
                    header.add_attribute_mapping("subtitle", "subtitle");

                    var image = scopes.lib.new_preview_widget("image", "image");
                    image.add_attribute_mapping("source", "art");

                    var description = scopes.lib.new_preview_widget("summary", "text");
                    description.add_attribute_mapping("text", "description");

                    preview_reply.push([image, header, description ]);
                    preview_reply.finished();
                },
                // cancelled
                function() {
                });
        }
    }
);

