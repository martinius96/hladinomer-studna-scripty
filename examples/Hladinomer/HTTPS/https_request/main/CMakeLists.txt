# Embed the server root certificate into the final binary
#
# (If this was a component, we would set COMPONENT_EMBED_TXTFILES here.)
idf_build_get_property(project_dir PROJECT_DIR)
idf_component_register(SRCS "https_request_example_main.c" "ultrasonic.c"
                    INCLUDE_DIRS "."
                    EMBED_TXTFILES ${project_dir}/server_certs/ca_cert.pem)