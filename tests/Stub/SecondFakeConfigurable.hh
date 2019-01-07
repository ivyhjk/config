<?hh // strict

namespace Ivyhjk\Test\Config\Stub;

class SecondFakeConfigurable extends FirstFakeConfigurable {
    protected static Map<string, mixed> $configurations = Map {
        'second' => Map {
            'config' => 2,
        },
    };
}
