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
 /* HH_FIXME[4123] */
class ConfigurableTest extends TestCase
{
    /**
     * Configurables can not be created by instance.
     *
     * @return void
     */
    public function test_constructor() : void
    {
        $reflection = new ReflectionMethod(SecondFakeConfigurable::class, '__construct');

        $this->assertFalse($reflection->isPublic());
    }

    /**
     * Configurables can not be created by cloned.
     *
     * @return void
     */
    public function test_clone() : void
    {
        $reflection = new ReflectionMethod(SecondFakeConfigurable::class, '__clone');

        $this->assertFalse($reflection->isPublic());
    }

    /**
     * Configurables can not be created by serialized.
     *
     * @return void
     */
    public function test_serialize() : void
    {
        $reflection = new ReflectionMethod(SecondFakeConfigurable::class, '__sleep');

        $this->assertFalse($reflection->isPublic());

        $this->expectException(LogicException::class);
        serialize(SecondFakeConfigurable::getInstance());
    }

    /**
     * settings can not be unserialized.
     *
     * @return void
     */
    public function test_unserialize() : void
    {
        $reflection = new ReflectionMethod(SecondFakeConfigurable::class, '__wakeup');
        $this->assertFalse($reflection->isPublic());

        $serialized = 'O:41:"Ivyhjk\Test\Config\SecondFakeConfigurable":0:{}';
        $this->expectException(LogicException::class);
        unserialize($serialized);
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
        $this->assertEquals($config->get('first'), Map{
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
        $this->assertSame($config->get('first.test'), 1);

        // depth 2, string
        $this->assertSame($config->get('first.string'), 'test');

        // depth 2, Vector
        $this->assertEquals($config->get('first.vector'), Vector{
            'one',
            'two',
            'three'
        });

        // depth 2, Vector, first value
        $this->assertSame($config->get('first.vector.1'), 'two');
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
        $this->assertSame($config->get('multiple.first.second.third.fourth'), 4);
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

        $this->assertSame($third->get('first.test'), 1);
        $this->assertSame($third->get('multiple.first.second.third.fourth'), 4);
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
        $this->assertNull($second->get('first.test'));
        $this->assertSame($second->get('second.config'), 2);
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

        $this->assertSame($firstFakeConfigurable->get($path), $value);
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
        $this->assertSame($firstFakeConfigurable->get($path), $value);
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
        $this->assertSame($firstFakeConfigurable->get($path), $value);
    }

    /**
     * Test the setter with an invalid path.
     *
     * @return void
     */
    public function test_setter_invalid_key() : void
    {
        $firstFakeConfigurable = FirstFakeConfigurable::getInstance();

        $this->expectException(InvalidArgumentException::class);
        $firstFakeConfigurable->set('first.string.fake', 'new-fake-value');
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
