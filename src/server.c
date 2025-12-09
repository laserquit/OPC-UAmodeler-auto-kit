/* This work is licensed under a Creative Commons CCZero 1.0 Universal License.
 * See http://creativecommons.org/publicdomain/zero/1.0/ for more information. */

#include <signal.h>
#include <stdio.h>
#include "open62541.h"

/* Files myNS.h and myNS.c are created from myNS.xml */
#include "myNS.h"

UA_Boolean running = true;

static void stopHandler(int sign) {
    UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_SERVER, "received ctrl-c");
    running = false;
}

// 这个方法的功能是把输入参数累加，传给输出参数
static UA_StatusCode helloWorldMethodCallback(UA_Server *server,
                         			const UA_NodeId *sessionId, void *sessionHandle,
                         			const UA_NodeId *methodId, void *methodContext,
                         			const UA_NodeId *objectId, void *objectContext,
                         			size_t inputSize, const UA_Variant *input,
                         			size_t outputSize, UA_Variant *output) 
{
    UA_Int32 value = 0;
    for (size_t i = 0; i < inputSize; ++i)
    {
    	UA_Int32 * ptr = (UA_Int32 *)input[i].data;
    	value += (*ptr);
    }

    UA_Variant_setScalarCopy(output, &value, &UA_TYPES[UA_TYPES_INT32]);
    
    UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_SERVER, "Hello World was called");
    
    return UA_STATUSCODE_GOOD;
}


int main(int argc, char **argv) 
{
    signal(SIGINT, stopHandler);
    signal(SIGTERM, stopHandler);

    UA_Server *server = UA_Server_new();
    UA_ServerConfig_setDefault(UA_Server_getConfig(server));

    UA_StatusCode retval;
    /* create nodes from nodeset */
    if (myNS(server) != UA_STATUSCODE_GOOD) {
        UA_LOG_ERROR(UA_Log_Stdout, UA_LOGCATEGORY_SERVER, "Could not add the example nodeset. "
            "Check previous output for any error.");
        retval = UA_STATUSCODE_BADUNEXPECTEDERROR;
    } else {
        
        // 方法节点的NodeId是UA_NODEID_NUMERIC(2, 7001)
        UA_Server_setMethodNode_callback(server, UA_NODEID_NUMERIC(2, 7001), &helloWorldMethodCallback);
        
        
        UA_NodeId createdNodeId;
        UA_ObjectAttributes object_attr = UA_ObjectAttributes_default;

        object_attr.description = UA_LOCALIZEDTEXT("en-US", "myNSObject");
        object_attr.displayName = UA_LOCALIZEDTEXT("en-US", "myNSObject");

        // we assume that the myNS nodeset was added in namespace 2.
        // You should always use UA_Server_addNamespace to check what the
        // namespace index is for a given namespace URI. UA_Server_addNamespace
        // will just return the index if it is already added.
        UA_Server_addObjectNode(server, UA_NODEID_NULL,
                                UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER),
                                UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES),
                                UA_QUALIFIEDNAME(1, "myNSObject"),
                                UA_NODEID_NUMERIC(2, 1002),
                                object_attr, NULL, &createdNodeId);
        
        
        
        UA_NodeId createdNodeId2;
        UA_ObjectAttributes object_attr2 = UA_ObjectAttributes_default;

        object_attr2.description = UA_LOCALIZEDTEXT("en-US", "myNSObject2");
        object_attr2.displayName = UA_LOCALIZEDTEXT("en-US", "myNSObject2");

        // we assume that the myNS nodeset was added in namespace 2.
        // You should always use UA_Server_addNamespace to check what the
        // namespace index is for a given namespace URI. UA_Server_addNamespace
        // will just return the index if it is already added.
        UA_Server_addObjectNode(server, UA_NODEID_NULL,
                                UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER),
                                UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES),
                                UA_QUALIFIEDNAME(1, "myNSObject2"),
                                UA_NODEID_NUMERIC(2, 1002),
                                object_attr2, NULL, &createdNodeId2);
        
        
        retval = UA_Server_run(server, &running);
    }
    UA_Server_delete(server);
    return (int) retval;
}
