<?hh // strict

namespace Ivyhjk\Test\Config\Stub;

use type Ivyhjk\Config\Configurable;

class FirstFakeConfigurable extends Configurable {
    protected static Map<string, mixed> $configurations = Map {
        'first' => Map {
            'test' => 1,
            'string' => 'test',
            'vector' => Vector {'one', 'two', 'three'},
        },
        'multiple' => Map {
            'first' => Map {
                'second' => Map {
                    'third' => Map {
                        'fourth' => 4,
                    },
                },
            },
        },
    };
}
