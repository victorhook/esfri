# Embed CA, certificate & key directly into binary
idf_component_register(
    SRC_DIRS "src"
    INCLUDE_DIRS "include"
    EMBED_TXTFILES certs/coap_ca.pem certs/coap_client.crt certs/coap_client.key
)
