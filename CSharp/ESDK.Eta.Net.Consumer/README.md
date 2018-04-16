# Consumer Application Description

# Summary

The purpose of this application is to consume content between an OMM
consumer and OMM provider. It is a single-threaded client application. The
general application flow is that it first initializes the ETA transport and
connects the client. After that, it sends a login request, loads dictionary
locally, and some item request messages to a provider and appropriately
processes the responses. The resulting responses from the provider are
displayed onto the console. This application does not send a source directory
request so users must know a service ID in order to consume data.  

This application supports consuming only Level I Market Price as normal streams.

# Configuration
All configurations parameters is defined in settings.json file and the ETA.Net.Consumer.csproj
project must be rebuilt for any changes.

# Setup Environment

The RDMFieldDictionary and enumtype.def files is located in the source directory of
the Consumer examples