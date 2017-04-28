const PROGMEM char joinTemplate[]={"<SSAP_message>\
<message_type>REQUEST</message_type>\
<transaction_type>JOIN</transaction_type>\
<transaction_id></transaction_id>\
<node_id></node_id>\
<space_id>X</space_id>\
</SSAP_message>"};

const PROGMEM char leaveTemplate[]={"<SSAP_message>\
<message_type>REQUEST</message_type>\
<transaction_type>LEAVE</transaction_type>\
<transaction_id></transaction_id>\
<node_id></node_id>\
<space_id>X</space_id>\
</SSAP_message>\
"};

const PROGMEM char insertTemplate[]={"<SSAP_message>\
<message_type>REQUEST</message_type>\
<transaction_type>INSERT</transaction_type>\
<transaction_id></transaction_id>\
<node_id></node_id>\
<space_id>X</space_id>\
<parameter name=\"confirm\">TRUE</parameter>\
<parameter name=\"insert_graph\"  encoding=\"RDF-M3\">\
<triple_list>\
</triple_list>\
</parameter>\
</SSAP_message>"};

const PROGMEM char queryTemplate[]={"<SSAP_message>\
<transaction_type>QUERY</transaction_type>\
<message_type>REQUEST</message_type>\
<transaction_id></transaction_id>\
<node_id></node_id>\
<space_id>X</space_id>\
<parameter name = \"type\">sparql</parameter>\
<parameter name = \"query\"></parameter>\
</SSAP_message>"};

const PROGMEM char subscribeTemplate[]={"<SSAP_message>\
<transaction_type>SUBSCRIBE</transaction_type>\
<message_type>REQUEST</message_type>\
<transaction_id></transaction_id>\
<node_id></node_id>\
<space_id>X</space_id>\
<parameter name = \"type\">sparql</parameter>\
<parameter name = \"query\"></parameter>\
</SSAP_message>"};

const PROGMEM char unsubscribeTemplate[]={"<SSAP_message>\
<transaction_type>UNSUBSCRIBE</transaction_type>\
<message_type>REQUEST</message_type>\
<transaction_id></transaction_id>\
<node_id></node_id>\
<space_id>X</space_id>\
<parameter name = \"subscription_id\"></parameter>\
</SSAP_message>"};

