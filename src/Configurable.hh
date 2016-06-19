<?hh // strict

namespace Ivyhjk\Config;

use LogicException;
use InvalidArgumentException;

/**
 * Configuration base class.
 *
 * @since v0.0.1
 * @version v0.0.1
 * @package Ivyhjk\Config
 * @author Elvis Munoz <elvis.munoz.f@gmail.com>
 * @copyright Copyright (c) 2016, Elvis Munoz
 * @licence https://opensource.org/licenses/MIT MIT License
 */
abstract class Configurable
{
    /**
     * Class unique instances pool.
     *
     * @var Map<string, Ivyhjk\Config\Configurable>
     */
    private static Map<string, Configurable> $instances = Map{};

    /**
     * Store all configurations.
     *
     * @var Map<string, mixed>
     */
    protected static Map<string, mixed> $configurations = Map{};

    /**
     * Constructor MUST be private, config.
     *
     * @return void
     */
    final protected function __construct() : void
    {

    }

    /**
     * The object can not be cloned.
     *
     * @return void
     */
    private function __clone() : void
    {

    }

    /**
     * The object can not be serialized.
     *
     * @return void
     */
    private function __sleep() : void
    {
        throw new LogicException('Object can not be serialized.');
    }

    /**
     * The object can not be unserialized.
     *
     * @return void
     */
    private function __wakeup() : void
    {
        throw new LogicException('Object can not be unserialized.');
    }

    /**
     * Get a settings object instance.
     *
     * @return Ivyhjk\Config\Configurable
     */
    final public static function getInstance() : Configurable
    {
        $class = get_called_class();
        $instance = self::$instances->get($class);

        if ($instance === null) {
            $instance = new static();
            self::$instances->set($class, $instance);
        }

        return $instance;
    }

    /**
     * Get the current configuration.
     *
     * @param mixed <null|string> $path Path to the configuration.
     *
     * @return mixed
     */
    public function get(?string $path = null) : mixed
    {
        if ($path === null) {
            return static::$configurations;
        }

        $paths = explode('.', $path);

        $node = static::$configurations;

        foreach ($paths as $path) {
            if ($node instanceof Map) {
                $node = $node->get($path);
            } else if ($node instanceof Vector && is_numeric($path)) {
                $node = $node->at((int) $path);
            } else {
                return $node;
            }
        }

        return $node;
    }

    /**
     * Description of what this does.
     *
     * @param mixed $myVar Short description
     *
     * @return void
     * @throws LogicException When key is not numeric on vector configuration.
     */
    public function set(string $path, mixed $value) : Configurable
    {
        $paths = explode('.', $path);

        $key = array_pop($paths);

        $relativePath = implode('.', $paths);

        $element = $this->get($relativePath);

        if ($element instanceof Map) {
            $element->set($key, $value);
        } else if ($element instanceof Vector) {
            if (is_numeric($key) !== true) {
                throw new InvalidArgumentException('Only integer keys may be used with Vectors');
            }

            $element->set((int) $key, $value);
        } else if ($element === null) {
            /*print_r(compact('key'));*/
            static::$configurations->set($key, $value);
        } else {
            throw new InvalidArgumentException(sprintf('Type %s has no values', gettype($element)));
        }

        return $this;
    }
}
