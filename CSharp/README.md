# Elektron Transport API .NET Edition

This is a proof of concept ETA .NET to provide pure .NET solution using the C# language for transport layer on consumer side to complement with
current offering with Java and C languages for ESDK API. This ESDK supports TCP/IP connectivity on consumer side, buffer management(such as read, write)
to read/write message from/to transport adapter. The public interfaces of ETA .NET Edition should have look and feel as same as Java or C Edition
unless there is any C# language features which provides greater experience for .NET developers.

# Software requirements
- .NET Standard 2.0 framework(inclused with VS2017) for the transport and codec library. This is Microsoft strategic .NET framework going forward. The ETA .NET edition is able
to run on support platforms for this framework.
- xUnit 2.3.1 or higher for unit testing.
- Microsoft.Extensions.Logging 2.0.0 or higher framework for logging
- .NET Core 2.0 for the ETA.Net.Consumer example.

# Building the Transport API

####1) Build the Transport API
Select the `csproj` for the specific library you want to build, or use the provided solution (or `sln`) file to build in **Visual Studio**. 

####2) Build the Transport API Examples

Navigate to `ESDK.Eta.Net.Consumer', to access the Consumer example. Open and build the windows solution file (when applicable) or the csproj.

# Transport API Features and Functionality

- Can consume:
    - Any and all OMM primitives supported on Elektron, Enterprise Platform, and Direct Exchange Feeds.
    - All Domain Models, including those defined by Thomson Reuters as well as other user-defined models.
- Consists of:
    - A transport-level API allowing for connectivity using TCP.  
    - OMM Encoder and Decoders, allowing full use of all OMM constructs and messages.
	
- Lack functionality
	- Message fragmentation.
	- Message Compression

# Notes:
- This package contains APIs that are subject to proprietary and opens source licenses.  Please make sure to read the README.md files within each package for clarification.
- Please make sure to review the LICENSE.md file.
- This ETA .NET is proof of concept for .NET framework implementation of Elektron Transport API and is not fully support. 