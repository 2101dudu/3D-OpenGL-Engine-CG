import os
import sys
import argparse
import random
import xml.etree.ElementTree as ET
import xml.dom.minidom as minidom


try:
    from PIL import Image
except ImportError:
    print("[-] Error: Pillow is required for compression. Install with `pip install pillow`.", file=sys.stderr)
    sys.exit(1)

TEXTURE_XML_PREFIX = "../../textures"  # base path in the XML files

def parse_args():
    parser = argparse.ArgumentParser(
        description="Generate solar-system XML, optionally compressing textures first."
    )
    parser.add_argument(
        "-c", "--compress",
        type=int, choices=range(1, 101), metavar="[1–100]",
        help="JPEG quality level for textures (1=worst,100=best)."
    )
    parser.add_argument(
        "-a", "--asteroids",
        type=int, choices=range(10, 10001), metavar="[10–10000]",
        default=1000,
        help="Number of asteroids per belt (10–10000)."
    )
    return parser.parse_args()

def human_readable_size(size_bytes):
    for unit in ('B','KB','MB','GB','TB'):
        if size_bytes < 1024:
            return f"{size_bytes:.2f} {unit}"
        size_bytes /= 1024
    return f"{size_bytes:.2f} PB"

def compress_textures(textures_dir, quality):
    subdir_name = f"compressed_{quality}"
    out_dir = os.path.join(textures_dir, subdir_name)
    os.makedirs(out_dir, exist_ok=True)

    total_before = 0
    total_after = 0

    for filename in os.listdir(textures_dir):
        if not filename.lower().endswith(".jpg"):
            continue
        src = os.path.join(textures_dir, filename)
        dst = os.path.join(out_dir, filename)
        total_before += os.path.getsize(src)
        try:
            with Image.open(src) as img:
                img.save(dst, format="JPEG", quality=quality, optimize=True)
        except Exception as e:
            print(f"[-] Failed to compress {src}: {e}", file=sys.stderr)
            sys.exit(1)
        total_after += os.path.getsize(dst)

    saved = total_before - total_after
    print(f"[+] Compressed textures to '{out_dir}'. Saved {human_readable_size(saved)} (from {human_readable_size(total_before)} to {human_readable_size(total_after)}).")
    return subdir_name


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


def create_model(file, textures=None, add_color=True, color_config=None):
    model = ET.Element("model")
    model.set("file", file)

    create_texture_element(model, texture_file_path=textures)

    if add_color:
        create_color_element(model, colors=color_config)
    
    return model


def create_texture_element(model_elem, texture_file_path=None):
    texture_elem = ET.SubElement(model_elem, "texture")

    if texture_file_path:
        fname = os.path.basename(texture_file_path)
    else:
        fname = "moon_low_res.jpg" 

    texture_elem.set("file", f"{TEXTURE_XML_PREFIX}/{fname}")

def create_group(name, clickableInfo, transform_data, model_file, color=None, texture=None):
    group = ET.Element("group")
    group.set("name", name)

    if clickableInfo != "":
        group.set("clickableInfo", clickableInfo)

    transform = create_transform(transform_data.get("rotate"),
                                 transform_data.get("translate"),
                                 transform_data.get("scale"))
    group.append(transform)
    models = ET.SubElement(group, "models")
    models.append(create_model(model_file, textures=texture, color_config=color))
    return group


def create_moon(moon_name, model_file, isEarth):
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

    if isEarth:
        return create_group(moon_name,  "../../group_info/earth_moon.txt", transform, model_file, texture=f"{TEXTURE_XML_PREFIX}/moon.jpg")

    return create_group(moon_name, "", transform, model_file)

def create_skybox_group():
    skybox_group = ET.Element("group")
    skybox_group.set("name", "Skybox")

    transform = ET.SubElement(skybox_group, "transform")
    scale = ET.SubElement(transform, "scale")
    scale.set("x", "-500.0")
    scale.set("y", "500.0")
    scale.set("z", "500.0")

    models = ET.SubElement(skybox_group, "models")
    models.append(create_model("../../objects/sphere.3d", textures=f"{TEXTURE_XML_PREFIX}/skybox.jpg", color_config=((100, 100, 150), (100, 100, 150), (0, 0, 0), (0, 0, 0), (0))))

    return skybox_group

def create_comet_group():
    comet_group = ET.Element("group")
    comet_group.set("name", "Comet")
    comet_group.set("clickableInfo", "../../group_info/comet.txt")

    transform = ET.SubElement(comet_group, "transform")
    translate = ET.SubElement(transform, "translate")
    translate.set("time", "50")
    translate.set("align", "True")

    points = [
        {"x": 40, "y": 0, "z": 0},
        {"x": 24, "y": 16, "z": 20},
        {"x": 10, "y": 30, "z": 10},
        {"x": -6, "y": 40, "z": 24},
        {"x": -20, "y": 20, "z": -10},
        {"x": -40, "y": 0, "z": 0},
        {"x": -20, "y": -20, "z": -30},
        {"x": 10, "y": -30, "z": -40},
        {"x": 24, "y": -16, "z": -20}
    ]

    for p in points:
        point = ET.SubElement(translate, "point")
        point.set("x", str(p["x"]))
        point.set("y", str(p["y"]))
        point.set("z", str(p["z"]))

    scale = ET.SubElement(transform, "scale")
    scale.set("x", "0.6")
    scale.set("y", "0.6")
    scale.set("z", "0.6")

    models = ET.SubElement(comet_group, "models")
    model = ET.SubElement(models, "model")
    model.set("file", "../../objects/comet.3d")

    create_color_element(model)

    return comet_group

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



def create_color_element(model_elem, colors=None, diffuse=(200,200,200), ambient=(50,50,70), specular=(255,255,255), emissive=(0,0,0), shininess=20):
    color = ET.SubElement(model_elem, "color")

    if colors is not None:
        (diffuse, ambient, specular, emissive, shininess) = colors

    d = ET.SubElement(color, "diffuse")
    d.set("R", str(diffuse[0]))
    d.set("G", str(diffuse[1]))
    d.set("B", str(diffuse[2]))

    a = ET.SubElement(color, "ambient")
    a.set("R", str(ambient[0]))
    a.set("G", str(ambient[1]))
    a.set("B", str(ambient[2]))

    s = ET.SubElement(color, "specular")
    s.set("R", str(specular[0]))
    s.set("G", str(specular[1]))
    s.set("B", str(specular[2]))

    e = ET.SubElement(color, "emissive")
    e.set("R", str(emissive[0]))
    e.set("G", str(emissive[1]))
    e.set("B", str(emissive[2]))

    sh = ET.SubElement(color, "shininess")
    sh.set("value", str(shininess))


def main():
    global TEXTURE_XML_PREFIX

    args = parse_args()

    # handle compression, if requested
    if args.compress:
        base_textures = os.path.join(os.getcwd(), "textures")
        subdir = compress_textures(base_textures, args.compress)
        TEXTURE_XML_PREFIX = f"../../textures/{subdir}"

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
    projection.set("near", "0.001")
    projection.set("far", "1000")

    # Lights definition
    lights = ET.SubElement(world, "lights")

    # Point light at the center (Sun)
    point_light = ET.SubElement(lights, "light")
    point_light.set("type", "point")
    point_light.set("posX", "0")
    point_light.set("posY", "0")
    point_light.set("posZ", "0")

    solar_system = ET.SubElement(world, "group")
    solar_system.set("name", "SolarSystem")

    # skybox
    skybox = create_skybox_group()
    solar_system.append(skybox)

    # Comet group
    comet = create_comet_group()
    solar_system.append(comet)

    # Sun (static at the center)
    sun_transform = {"rotate": {"time": 100,
                                "x": 0, "y": 1, "z": 0}, "translate": None, "scale": {"x": 3, "y": 3, "z": 3}}
    sun = create_group("Sun", "../../group_info/sun.txt", sun_transform, "../../objects/sphere.3d", color=((200,200,200), (200,200,200), (200,200,200), (200,200,200), (0)), texture=f"{TEXTURE_XML_PREFIX}/sun.jpg")
    solar_system.append(sun)

    # Planet data: (name, texture, info, distance, scale)
    planet_data = [
        ("Mercury", f"{TEXTURE_XML_PREFIX}/mercury.jpg", "../../group_info/mercury.txt", 10, 0.04),
        ("Venus", f"{TEXTURE_XML_PREFIX}/venus.jpg", "../../group_info/venus.txt", 15, 0.1),
        ("Earth", f"{TEXTURE_XML_PREFIX}/earth.jpg", "../../group_info/earth.txt", 20, 0.11),
        ("Mars", f"{TEXTURE_XML_PREFIX}/mars.jpg", "../../group_info/mars.txt", 27, 0.06),
        ("Jupiter",f"{TEXTURE_XML_PREFIX}/jupiter.jpg", "../../group_info/jupiter.txt", 45, 1.3),
        ("Saturn", f"{TEXTURE_XML_PREFIX}/saturn.jpg", "../../group_info/saturn.txt", 60, 0.7),
        ("Uranus",f"{TEXTURE_XML_PREFIX}/uranus.jpg", "../../group_info/uranus.txt", 75, 0.42),
        ("Neptune", f"{TEXTURE_XML_PREFIX}/neptune.jpg", "../../group_info/neptune.txt", 90, 0.41)
    ]

    # Number of moons for each planet
    moons_per_planet = {
        "Mercury": 0, "Venus": 0,
        "Earth": 1, "Mars": 2,
        "Jupiter": 4, "Saturn": 5,
        "Uranus": 3, "Neptune": 2
    }

    for planet, texture, clickableInfo, distance, scale in planet_data:
        # Orbital distribution: uses a random time per full rotation
        time = random.uniform(10, 50)
        transform = {
            "rotate": {"time": time, "x": 0, "y": 1, "z": 0},
            "translate": {"x": distance, "y": 0, "z": 0},
            "scale": {"x": scale, "y": scale, "z": scale}
        }
        planet_group = create_group(
            planet, clickableInfo, transform, "../../objects/sphere.3d", texture=texture)
        # Adds moons randomly
        for i in range(moons_per_planet[planet]):
            if planet == "Earth":
                moon = create_moon(f"{planet}_Moon_{i+1}",
                               "../../objects/sphere.3d", True)
            else:
                moon = create_moon(f"{planet}_Moon_{i+1}",
                                "../../objects/sphere.3d", False)
            planet_group.append(moon)
        solar_system.append(planet_group)

    # Inner asteroid belt (farther from Mars to avoid overlap)
    add_asteroid_belt(solar_system, num_asteroids=args.asteroids,
                      min_dist=32, max_dist=38)
    # Outer asteroid belt (closer to the Sun)
    add_asteroid_belt(solar_system, num_asteroids=args.asteroids,
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
            ring_models.append(create_model("../../objects/torus.3d", textures=f"{TEXTURE_XML_PREFIX}/ring.jpg"))
            group.append(ring_group)
            break

    pretty_xml = prettify_xml(world)
    with open("config.xml", "w", encoding="utf-8") as f:
        f.write(pretty_xml)

    print("[+] Solar system XML successfully generated in 'config.xml'!")


if __name__ == "__main__":
    main()
