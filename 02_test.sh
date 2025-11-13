bazel test --config bl-x86_64-linux -- //score/... \
            -//score/language/safecpp/aborts_upon_exception:abortsuponexception_toolchain_test \
            -//score/containers:dynamic_array_test
