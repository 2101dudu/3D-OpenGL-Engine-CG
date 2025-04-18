import xml.etree.ElementTree as ET
import xml.dom.minidom as minidom
import random


def create_transform(rotate, translate, scale):
    transform = ET.Element("transform")
    if rotate:
        r = ET.SubElement(transform, "rotate")
        r.set("time", str(rotate["time"]))
        r.set("x", str(rotate["x"]))
        r.set("y", str(rotate["y"]))
        r.set("z", str(rotate["z"]))
    if translate:
        t = ET.SubElement(transform, "translate")
        t.set("x", str(translate["x"]))
        t.set("y", str(translate["y"]))
        t.set("z", str(translate["z"]))
    if scale:
        s = ET.SubElement(transform, "scale")
        s.set("x", str(scale["x"]))
        s.set("y", str(scale["y"]))
        s.set("z", str(scale["z"]))
    return transform


def create_model(file):
    model = ET.Element("model")
    model.set("file", file)
    return model


def create_group(name, transform_data, model_file):
    group = ET.Element("group")
    group.set("name", name)
    transform = create_transform(transform_data.get("rotate"),
                                 transform_data.get("translate"),
                                 transform_data.get("scale"))
    group.append(transform)
    models = ET.SubElement(group, "models")
    models.append(create_model(model_file))
    return group


def create_moon(moon_name, model_file):
    # Generates a random number to use as the time per full rotation
    time = random.uniform(0, 30)
    moon_distance = random.uniform(1.5, 3.5)
    y_offset = random.uniform(-0.5, 0.5)
    scale_factor = random.uniform(0.02, 0.05)
    transform = {
        "rotate": {"time": time, "x": 0, "y": 1, "z": 0},
        "translate": {"x": moon_distance, "y": y_offset, "z": 0},
        "scale": {"x": scale_factor, "y": scale_factor, "z": scale_factor}
    }
    return create_group(moon_name, transform, model_file)


def add_asteroid_belt(parent, num_asteroids, min_dist, max_dist):
    # Creates an asteroid belt with random positions and scales
    belt = ET.Element("group")
    belt.set("name", f"AsteroidBelt_{min_dist}_{max_dist}")
    for i in range(num_asteroids):
        asteroid = ET.Element("group")
        asteroid.set("name", f"Asteroid_{i}")
        time = random.uniform(0, 50)
        distance = random.uniform(min_dist, max_dist)
        y_variation = random.uniform(-2, 2)
        transform = ET.Element("transform")
        r = ET.SubElement(transform, "rotate")
        r.set("time", str(time))
        r.set("x", "0")
        r.set("y", "1")
        r.set("z", "0")
        t = ET.SubElement(transform, "translate")
        t.set("x", str(distance))
        t.set("y", str(y_variation))
        t.set("z", "0")
        s = ET.SubElement(transform, "scale")
        scale_val = random.uniform(0.005, 0.02)
        s.set("x", str(scale_val))
        s.set("y", str(scale_val))
        s.set("z", str(scale_val))
        asteroid.append(transform)
        models = ET.SubElement(asteroid, "models")
        models.append(create_model("../../objects/sphere.3d"))
        belt.append(asteroid)
    parent.append(belt)


def prettify_xml(elem):
    rough_string = ET.tostring(elem, 'utf-8')
    reparsed = minidom.parseString(rough_string)
    return reparsed.toprettyxml(indent="    ")


def main():
    world = ET.Element("world")

    # Window definition
    window = ET.SubElement(world, "window")
    window.set("width", "1080")
    window.set("height", "720")

    # Camera definition
    camera = ET.SubElement(world, "camera")
    pos = ET.SubElement(camera, "position")
    pos.set("x", "10")
    pos.set("y", "10")
    pos.set("z", "10")
    lookAt = ET.SubElement(camera, "lookAt")
    lookAt.set("x", "0")
    lookAt.set("y", "0")
    lookAt.set("z", "0")
    up = ET.SubElement(camera, "up")
    up.set("x", "0")
    up.set("y", "1")
    up.set("z", "0")
    projection = ET.SubElement(camera, "projection")
    projection.set("fov", "60")
    projection.set("near", "0.01")
    projection.set("far", "1000")

    solar_system = ET.SubElement(world, "group")
    solar_system.set("name", "SolarSystem")

    # Sun (static at the center)
    sun_transform = {"rotate": {"time": 100,
                                "x": 0, "y": 1, "z": 0}, "translate": None, "scale": {"x": 3, "y": 3, "z": 3}}
    sun = create_group("Sun", sun_transform, "../../objects/sphere.3d")
    solar_system.append(sun)

    # Planet data: (name, distance, scale)
    planet_data = [
        ("Mercury", 10, 0.04),
        ("Venus", 15, 0.1),
        ("Earth", 20, 0.11),
        ("Mars", 27, 0.06),
        ("Jupiter", 45, 1.3),
        ("Saturn", 60, 0.7),
        ("Uranus", 75, 0.42),
        ("Neptune", 90, 0.41)
    ]

    # Number of moons for each planet
    moons_per_planet = {
        "Mercury": 0, "Venus": 0,
        "Earth": 1, "Mars": 2,
        "Jupiter": 4, "Saturn": 5,
        "Uranus": 3, "Neptune": 2
    }

    for planet, distance, scale in planet_data:
        # Orbital distribution: uses a random time per full rotation
        time = random.uniform(0, 50)
        transform = {
            "rotate": {"time": time, "x": 0, "y": 1, "z": 0},
            "translate": {"x": distance, "y": 0, "z": 0},
            "scale": {"x": scale, "y": scale, "z": scale}
        }
        planet_group = create_group(
            planet, transform, "../../objects/sphere.3d")
        # Adds moons randomly
        for i in range(moons_per_planet[planet]):
            moon = create_moon(f"{planet}_Moon_{i+1}",
                               "../../objects/sphere.3d")
            planet_group.append(moon)
        solar_system.append(planet_group)

    # Inner asteroid belt (farther from Mars to avoid overlap)
    add_asteroid_belt(solar_system, num_asteroids=1000,
                      min_dist=32, max_dist=38)
    # Outer asteroid belt (closer to the Sun)
    add_asteroid_belt(solar_system, num_asteroids=1000,
                      min_dist=120, max_dist=140)

    # Finds the Saturn group and inserts the ring as a subgroup right after its creation
    for group in solar_system.findall("group"):
        if group.get("name") == "Saturn":
            ring_group = ET.Element("group")
            ring_group.set("name", "ring")
            ring_transform = ET.Element("transform")
            r = ET.SubElement(ring_transform, "rotate")
            r.set("angle", "35")
            r.set("x", "1")
            r.set("y", "0")
            r.set("z", "0")
            s = ET.SubElement(ring_transform, "scale")
            s.set("x", "0.7")
            s.set("y", "0.1")
            s.set("z", "0.7")
            ring_group.append(ring_transform)
            ring_models = ET.SubElement(ring_group, "models")
            ring_models.append(create_model("../../objects/torus.3d"))
            group.append(ring_group)
            break

    pretty_xml = prettify_xml(world)
    with open("config.xml", "w", encoding="utf-8") as f:
        f.write(pretty_xml)

    print("Solar system XML successfully generated in 'config.xml'!")


if __name__ == "__main__":
    main()
