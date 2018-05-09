<?hh // strict

namespace Ivyhjk\Test\Config;

use LogicException;
use ReflectionMethod;
use InvalidArgumentException;
use PHPUnit\Framework\TestCase;
use Ivyhjk\Config\Configurable;

/**
 * Short description
 *
 * @since v0.0.1
 * @version v0.0.1
 * @package Ivyhjk\Test\Config
 * @author Elvis Munoz <elvis.munoz.f@gmail.com>
 * @copyright Copyright (c) 2016, Elvis Munoz
 * @licence https://opensource.org/licenses/MIT MIT License
 */
 /* HH_FIXME[4123] */ /* HH_FIXME[2049] */
class ConfigurableTest extends \PHPUnit_Framework_TestCase
{
    /**
     * Configurables can not be created by instance.
     *
     * @return void
     */
    public function test_constructor() : void
    {
        $reflection = new ReflectionMethod(SecondFakeConfigurable::class, '__construct');

        static::assertFalse($reflection->isPublic());
    }

    /**
     * Configurables can not be created by cloned.
     *
     * @return void
     */
    public function test_clone() : void
    {
        $reflection = new ReflectionMethod(SecondFakeConfigurable::class, '__clone');

        static::assertFalse($reflection->isPublic());
    }

    /**
     * Configurables can not be created by serialized.
     *
     * @return void
     */
    public function test_serialize() : void
    {
        $reflection = new ReflectionMethod(SecondFakeConfigurable::class, '__sleep');

        static::assertFalse($reflection->isPublic());

        $this->expectException(LogicException::class);
        \serialize(SecondFakeConfigurable::getInstance());
    }

    /**
     * settings can not be unserialized.
     *
     * @return void
     */
    public function test_unserialize() : void
    {
        $reflection = new ReflectionMethod(SecondFakeConfigurable::class, '__wakeup');
        static::assertFalse($reflection->isPublic());

        $serialized = 'O:41:"Ivyhjk\Test\Config\SecondFakeConfigurable":0:{}';
        $this->expectException(LogicException::class);
        \unserialize($serialized);
    }

    /**
     * Description of what this does.
     *
     * @param mixed $myVar Short description
     *
     * @return void
     */
    public function test_getter_first_deep() : void
    {
        $config = FirstFakeConfigurable::getInstance();

        // depth 1, any
        static::assertEquals($config->get('first'), Map{
            'test' => 1,
            'string' => 'test',
            'vector' => Vector{'one', 'two', 'three'}
        });
    }

    /**
     * Description of what this does.
     *
     * @param mixed $myVar Short description
     *
     * @depends test_getter_first_deep
     * @return void
     */
    public function test_getter_second_deep() : void
    {
        $config = FirstFakeConfigurable::getInstance();

        // depth 2, int
        static::assertSame($config->get('first.test'), 1);

        // depth 2, string
        static::assertSame($config->get('first.string'), 'test');

        // depth 2, Vector
        static::assertEquals($config->get('first.vector'), Vector{
            'one',
            'two',
            'three'
        });

        // depth 2, Vector, first value
        static::assertSame($config->get('first.vector.1'), 'two');
    }

    /**
     * Test the configurations getter.
     *
     * @depends test_getter_second_deep
     * @return void
     */
    public function test_getter_multiple_deepth() : void
    {
        $config = FirstFakeConfigurable::getInstance();

        // really depth.
        static::assertSame($config->get('multiple.first.second.third.fourth'), 4);
    }

    /**
     * Check if inheritance consistence.
     * Configs from ThirdFakeConfigurable MUST be the same as FirstFakeConfigurable.
     *
     * @depends test_getter_multiple_deepth
     * @return void
     */
    public function test_correct_inheritance() : void
    {
        $third = ThirdFakeConfigurable::getInstance();

        static::assertSame($third->get('first.test'), 1);
        static::assertSame($third->get('multiple.first.second.third.fourth'), 4);
    }

    /**
     * Check if inheritance consistence.
     * Configs from SecondFakeConfigurable MUST override FirstFakeConfigurable setting.
     *
     * @depends test_getter_multiple_deepth
     * @return void
     */
    public function test_correct_inheritance_override() : void
    {
        $second = SecondFakeConfigurable::getInstance();
        static::assertNull($second->get('first.test'));
        static::assertSame($second->get('second.config'), 2);
    }

    /**
     * Test the configuration setter into an existent map.
     *
     * @return void
     */
    public function test_setter_existent_map() : void
    {
        $firstFakeConfigurable = FirstFakeConfigurable::getInstance();

        $path = 'first.new';
        $value = 'new-value';

        $firstFakeConfigurable->set($path, $value);

        static::assertSame($firstFakeConfigurable->get($path), $value);
    }

    /**
     * Test the configuration setter into an existent vector.
     * An error is expected here.
     *
     * @depends test_getter_first_deep
     * @return void
     */
    public function test_setter_existent_vector_invalid() : void
    {
        $firstFakeConfigurable = FirstFakeConfigurable::getInstance();

        $this->expectException(InvalidArgumentException::class);
        $firstFakeConfigurable->set('first.vector.new', 'new-value');
    }

    /**
     * Test the configuration setter into an existent vector.
     *
     * @param mixed $myVar Short description
     *
     * @depends test_getter_first_deep
     * @return void
     */
    public function test_setter_existent_vector_valid() : void
    {
        $firstFakeConfigurable = FirstFakeConfigurable::getInstance();

        $path = 'first.vector.1';
        $value = 'new-two-value';

        $firstFakeConfigurable->set($path, $value);
        static::assertSame($firstFakeConfigurable->get($path), $value);
    }

    /**
     * Test a new Map/Vector path for a inexistent path.
     * Test just for first deep creation.
     *
     * @return void
     */
    public function test_setter_new_creation_first_deep() : void
    {
        $firstFakeConfigurable = FirstFakeConfigurable::getInstance();

        $path = 'new-path';
        $value = 'new-path-value';

        $firstFakeConfigurable->set($path, $value);
        static::assertSame($firstFakeConfigurable->get($path), $value);
    }

    /**
     * Test the setter with multiple values on path input and value input.
     *
     * @return void
     */
    public function test_setter_many_key_values() : void
    {
        $firstFakeConfigurable = FirstFakeConfigurable::getInstance();

        $firstFakeConfigurable->set('first.string.fake', 'new-fake-value');
        static::assertSame(
            'new-fake-value',
            $firstFakeConfigurable->get('first.string.fake')
        );
    }

    /**
     * Test the correct workflow for prepareForSet method.
     * This test contemplate a non string keys given.
     *
     * @return void
     */
    public function test_prepare_for_set_non_string_keys() : void
    {
        $firstFakeConfigurable = FirstFakeConfigurable::getInstance();

        $keys = ['foo' => 'bar'];

        static::assertSame($keys, $firstFakeConfigurable->prepareForSet($keys, 'baz'));
    }

    /**
     * Test the correct workflow for prepareForSet method.
     * This test contemplate an existent "final value".
     *
     * @return void
     */
    public function test_prepare_for_set_with_final_value() : void
    {
        $firstFakeConfigurable = FirstFakeConfigurable::getInstance();

        $value = $firstFakeConfigurable->prepareForSet('foo', 'bar', 'baz');

        $expected = Map{
            'foo' => Map{
                'bar' => 'baz'
            }
        };

        static::assertEquals($expected, $value);
    }

    /**
     * Test the correct workflow for prepareForSet method.
     * This test contemplates a single key (a key without dot)
     *
     * @return void
     */
    public function test_prepare_for_set_with_one_key() : void
    {
        $firstFakeConfigurable = FirstFakeConfigurable::getInstance();

        $value = $firstFakeConfigurable->prepareForSet('foo', 'bar');

        $expected = Map{
            'foo' => 'bar'
        };

        static::assertEquals($expected, $value);
    }

    /**
     * Test the correct workflow for prepareForSet method.
     * This test contemplates a multiple key (a key with dots)
     *
     * @return void
     */
    public function test_prepare_for_set_with_multiple_key() : void
    {
        $firstFakeConfigurable = FirstFakeConfigurable::getInstance();

        $value = $firstFakeConfigurable->prepareForSet(
            'foo.bar.baz',
            'fake-value'
        );

        $expected = Map{
            'foo' => Map{
                'bar' => Map{
                    'baz' => 'fake-value'
                }
            }
        };

        static::assertEquals($expected, $value);
    }
}

class FirstFakeConfigurable extends Configurable
{
    protected static Map<string, mixed> $configurations = Map{
        'first' => Map{
            'test' => 1,
            'string' => 'test',
            'vector' => Vector{'one', 'two', 'three'}
        },
        'multiple' => Map{
            'first' => Map {
                'second' => Map{
                    'third' => Map {
                        'fourth' => 4
                    }
                }
            }
        }
    };
}

class SecondFakeConfigurable extends FirstFakeConfigurable
{
    protected static Map<string, mixed> $configurations = Map{
        'second' => Map{
            'config' => 2
        }
    };
}

class ThirdFakeConfigurable extends FirstFakeConfigurable
{

}
