# Learning Nugget "How to setup a System where I can run SCORE-Code"

## Prerequisites

### Install github cli
Install github command line interface.

`sudo apt install gh`

### Install github extension

Execute the following command in your shell to install the github extension.

`gh extension install etas-contrib/gh-contrib-clone`

The source code of the extension can be found in this repo:
https://github.com/etas-contrib/gh-contrib-clone/blob/main/gh-contrib-clone

## Clone the repo

In order to directly have the right remotes, use the github extension to clone the repository you want to work with.

`gh contrib-clone eclipse-score/<repo_name>`

e.g. 

`gh contrib-clone eclipse-score/baselibs`

## Open and setup the IDE

Change into the directory of the repo you have just cloned.

`cd baselibs`

and open Visual Studio Code

`code .`

Open the folder in a Dev container by either clicking on the "Reopen in container" button

![Reopen in container](./reopen_in_container.png)

or by Ctrl+Shift+P "Dev Containers: Open in Container".    

# Build and test the module

Open a terminal in VS Code and enter

`bazel build --config=bl-x86_64-linux -c dbg //...`

This will build the module.

Now that we have built everything, we can execute the tests with

`bazel test --config bl-x86_64-linux -- //...  -//score/language/safecpp/aborts_upon_exception:abortsuponexception_toolchain_test -//score/containers:dynamic_array_test`


Mit baselibs anfangen, mit Bazel bauen, JVM Code in devcontainer JSON (siehe https://dev.azure.com/bosch-denso-common/VRTE-Communication/_git/score-someip-gateway?path=%2F.devcontainer%2Fetas%2Fdevcontainer.json&version=GBprototype&_a=contents)

Änderung machen, nach etas-contrib pushen, PR nach score machen.

Bazel local path overwrite

Unit tests laufen lassen (bazel test ...)

Documentation erstellen!!!


 
startup --host_jvm_args=-Djavax.net.ssl.trustStore=/etc/ssl/certs/java/cacerts \
        --host_jvm_args=-Djavax.net.ssl.trustStorePassword=changeit
 
"common --disk_cache=/home/runner/.cache/bazel-disk",
"common --repository_cache=/home/runner/.cache/bazel-repo" 