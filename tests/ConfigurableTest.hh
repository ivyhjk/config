<?hh // strict

namespace Ivyhjk\Test\Config;

use function Facebook\FBExpect\expect;

/**
 * Configurable tests.

 * @since     v1.0.0
 * @version   v1.1.0
 * @package   Ivyhjk\Test\Config
 * @author    Elvis Munoz <elvis.munoz.f@gmail.com>
 * @copyright Copyright (c) 2016, Elvis Munoz
 * @license   https://opensource.org/licenses/MIT MIT License
 */
class ConfigurableTest extends \Facebook\HackTest\HackTest {
    /**
     * Configurables can not be created by instance.
     *
     * @return void
     */
    public async function testConstructor(): Awaitable<void> {
        $reflection = new \ReflectionMethod(
            Stub\SecondFakeConfigurable::class,
            '__construct',
        );

        expect($reflection->isPublic())->toBeFalse();
    }

    /**
     * Configurables can not be created by cloned.
     *
     * @return void
     */
    public async function testClone(): Awaitable<void> {
        $reflection = new \ReflectionMethod(
            Stub\SecondFakeConfigurable::class,
            '__clone',
        );

        expect($reflection->isPublic())->toBeFalse();
    }

    /**
     * Configurables can not be created by serialized.
     *
     * @return void
     */
    <<ExpectedException(\LogicException::class)>>
    public async function testSerialize(): Awaitable<void> {
        $reflection = new \ReflectionMethod(
            Stub\SecondFakeConfigurable::class,
            '__sleep',
        );

        expect($reflection->isPublic())->toBeFalse();

        \serialize(Stub\SecondFakeConfigurable::getInstance());
    }

    /**
     * @return void
     */
    public async function testGetterFirstDeep(): Awaitable<void> {
        $config = Stub\FirstFakeConfigurable::getInstance();

        // depth 1, any
        expect($config->get('first'))->toBePHPEqual(Map {
            'test' => 1,
            'string' => 'test',
            'vector' => Vector {'one', 'two', 'three'},
        });
    }

    /**
     * @return void
     */
    public async function testGetterSecondDeep(): Awaitable<void> {
        $config = Stub\FirstFakeConfigurable::getInstance();

        // depth 2, int
        expect($config->get('first.test'))->toBeSame(1);

        // depth 2, string
        expect($config->get('first.string'))->toBeSame('test');

        // depth 2, Vector
        expect($config->get('first.vector'))->toBePHPEqual(Vector {
            'one',
            'two',
            'three',
        });

        // depth 2, Vector, first value
        expect($config->get('first.vector.1'))->toBeSame('two');
    }

    /**
     * Test the configurations getter.
     *
     * @return void
     */
    public async function testGetterMultipleDeepth(): Awaitable<void> {
        $config = Stub\FirstFakeConfigurable::getInstance();

        // really depth.
        expect($config->get('multiple.first.second.third.fourth'))->toBeSame(4);
    }

    /**
     * Check if inheritance consistence.
     * Configs from ThirdFakeConfigurable MUST be the same as FirstFakeConfigurable.
     *
     * @return void
     */
    public async function testCorrectInheritance(): Awaitable<void> {
        $third = Stub\ThirdFakeConfigurable::getInstance();

        expect($third->get('first.test'))->toBeSame(1);
        expect($third->get('multiple.first.second.third.fourth'))->toBeSame(4);
    }

    /**
     * Check if inheritance consistence.
     * Configs from SecondFakeConfigurable MUST override FirstFakeConfigurable setting.
     *
     * @return void
     */
    public async function testCorrectInheritanceOverride(): Awaitable<void> {
        $second = Stub\SecondFakeConfigurable::getInstance();
        expect($second->get('first.test'))->toBeNull();
        expect($second->get('second.config'))->toBeSame(2);
    }

    /**
     * Test the configuration setter into an existent map.
     *
     * @return void
     */
    public async function testSetterExistentMap(): Awaitable<void> {
        $firstFakeConfigurable = Stub\FirstFakeConfigurable::getInstance();

        $path = 'first.new';
        $value = 'new-value';

        $firstFakeConfigurable->set($path, $value);

        expect($firstFakeConfigurable->get($path))->toBeSame($value);
    }

    /**
     * Test the configuration setter into an existent vector.
     * An error is expected here.
     *
     * @return void
     */
    <<ExpectedException(\InvalidArgumentException::class)>>
    public async function testSetterExistentVectorInvalid(): Awaitable<void> {
        $firstFakeConfigurable = Stub\FirstFakeConfigurable::getInstance();

        $firstFakeConfigurable->set('first.vector.new', 'new-value');
    }

    /**
     * Test the configuration setter into an existent vector.
     *
     * @param mixed $myVar Short description
     *
     * @return void
     */
    public async function testSetterExistentVectorValid(): Awaitable<void> {
        $firstFakeConfigurable = Stub\FirstFakeConfigurable::getInstance();

        $path = 'first.vector.1';
        $value = 'new-two-value';

        $firstFakeConfigurable->set($path, $value);

        expect($firstFakeConfigurable->get($path))->toBeSame($value);
    }

    /**
     * Test a new Map/Vector path for a inexistent path.
     * Test just for first deep creation.
     *
     * @return void
     */
    public async function testSetterNewCreationFirstDeep(): Awaitable<void> {
        $firstFakeConfigurable = Stub\FirstFakeConfigurable::getInstance();

        $path = 'new-path';
        $value = 'new-path-value';

        $firstFakeConfigurable->set($path, $value);

        expect($firstFakeConfigurable->get($path))->toBeSame($value);
    }

    /**
     * Test the setter with multiple values on path input and value input.
     *
     * @return void
     */
    public async function testSetterManyKeyValues(): Awaitable<void> {
        $firstFakeConfigurable = Stub\FirstFakeConfigurable::getInstance();

        $firstFakeConfigurable->set('first.string.fake', 'new-fake-value');

        expect($firstFakeConfigurable->get('first.string.fake'))->toBeSame(
            'new-fake-value',
        );
    }

    /**
     * Test the correct workflow for prepareForSet method.
     * This test contemplate a non string keys given.
     *
     * @return void
     */
    public async function testPrepareForSetNonStringKeys(): Awaitable<void> {
        $firstFakeConfigurable = Stub\FirstFakeConfigurable::getInstance();

        $keys = dict['foo' => 'bar'];

        expect($firstFakeConfigurable->prepareForSet($keys, 'baz'))->toBeSame(
            $keys,
        );
    }

    /**
     * Test the correct workflow for prepareForSet method.
     * This test contemplate an existent "final value".
     *
     * @return void
     */
    public async function testPrepareForSetWithFinalValue(): Awaitable<void> {
        $firstFakeConfigurable = Stub\FirstFakeConfigurable::getInstance();

        $value = $firstFakeConfigurable->prepareForSet('foo', 'bar', 'baz');

        $expected = Map {
            'foo' => Map {
                'bar' => 'baz',
            },
        };

        expect($value)->toBePHPEqual($expected);
    }

    /**
     * Test the correct workflow for prepareForSet method.
     * This test contemplates a single key (a key without dot)
     *
     * @return void
     */
    public async function testPrepareForSetWithOneKey(): Awaitable<void> {
        $firstFakeConfigurable = Stub\FirstFakeConfigurable::getInstance();

        $value = $firstFakeConfigurable->prepareForSet('foo', 'bar');

        $expected = Map {
            'foo' => 'bar',
        };

        expect($value)->toBePHPEqual($expected);
    }

    /**
     * Test the correct workflow for prepareForSet method.
     * This test contemplates a multiple key (a key with dots)
     *
     * @return void
     */
    public async function testPrepareForSetWithMultipleKey(): Awaitable<void> {
        $firstFakeConfigurable = Stub\FirstFakeConfigurable::getInstance();

        $value =
            $firstFakeConfigurable->prepareForSet('foo.bar.baz', 'fake-value');

        $expected = Map {
            'foo' => Map {
                'bar' => Map {
                    'baz' => 'fake-value',
                },
            },
        };

        expect($value)->toBePHPEqual($expected);
    }
}
