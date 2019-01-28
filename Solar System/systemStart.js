/*
 * NAMES: 	Jagandeep Brar, Jada Cheung, Ullas Gurukumar, Gareth Wiebe
 * STID: 	7739801, 7752754, 7747220, 7789614
 * COURSE: 	COMP3490
 * PROF:  	Neil Bruce
 * DATE: 	December 20th, 2017
 */

//Global Constants
const EARTH_RADIUS = 50;
const SUN_RADIUS = EARTH_RADIUS*10;

const OFFSET_X = -1120;
const OFFSET_Y = 300;
const OFFSET_PLANETS = 120;

const SUN_X = 0;
const SUN_Y = OFFSET_Y;
const SUN_Z = 0;

const GRAVITY_CONSTANT = 1;
const NUM_OF_ASTEROIDS = 25;
const ASSET_URL_PREFIX = "./assets/";

//Global Variables -- General
var sunLight, lensFlare;
var lastCamX, lastCamY, lastCamZ;
var skybox;
var clock = new THREE.Clock();
var input = "";


//Global Variables -- Three.js
var camera, scene, renderer;
var farClipping = 25000;
var cameraControls;

//Proton js (Particle Engine)
var proton, emitter;
var maxParticles = 2;

//Global Variables -- Stats.js and Dat.GUI
var gui;
var stats;

//Global Variables -- Changable via Dat.GUI
var rotationSpeed = 5;
var orbitSpeed = 5;
var fpsMonitor = true;

//Global Variables -- Physics Engine
var margin = 0.05;
var delta2 = 0.00;
var collisionConfiguration;
var dispatcher;
var broadphase;
var solver;
var physicsWorld;
var terrainBody;
var dynamicObjects = [];
var transformAux1 = new Ammo.btTransform();
var dynamicObjectsMass = [];

//Global object to hold planet data
//Planet data was retrieved from: https://www.windows2universe.org/our_solar_system/planets_table.html
var planetObjects = {
	Mercury	: { 
		name: "Mercury",
		radius: EARTH_RADIUS*0.38,
		distance: (OFFSET_X-(OFFSET_PLANETS*0)),
		tilt: calculateTilt(0.03),
		rotationSpeed: 58.6462,
		rotationSpeedScale: 1,
		orbitInclination: calculateTilt(3.38),
		orbitSpeed: 0.241,
		orbitSpeedScale: 1,
		textureMap: ASSET_URL_PREFIX+"mercury/mercurymap.jpg", 
		bumpMap: ASSET_URL_PREFIX+"mercury/mercurybump.jpg",
		colour: 0xd4b89d,
		planet: null,
		orbit: null,
		group: new THREE.Group()},
	Venus : { 
		name: "Venus",
		radius: EARTH_RADIUS*0.95,
		distance: (OFFSET_X-(OFFSET_PLANETS*1.5)),
		tilt: calculateTilt(2.64),
		rotationSpeed: 243.0187,
		rotationSpeedScale: 1,
		orbitInclination: calculateTilt(3.86),
		orbitSpeed: 0.6152,
		orbitSpeedScale: 1,
		textureMap: ASSET_URL_PREFIX+"venus/venusmap.jpg",
		bumpMap: ASSET_URL_PREFIX+"venus/venusbump.jpg",
		colour: 0x6e0c0c,
		planet: null,
		orbit: null,
		group: new THREE.Group()},
	Earth : {
		name: "Earth",
		radius: EARTH_RADIUS,
		distance: (OFFSET_X-(OFFSET_PLANETS*3)),
		tilt: calculateTilt(23.44),
		rotationSpeed: 1.0000,
		rotationSpeedScale: 1,
		orbitInclination: calculateTilt(7.16),
		orbitSpeed: 1.0000,
		orbitSpeedScale: 1,
		textureMap: ASSET_URL_PREFIX+"earth/earthmap.jpg", 
		bumpMap: ASSET_URL_PREFIX+"earth/earthbump.jpg",	
		atmosMap: ASSET_URL_PREFIX+"earth/earthatmos.jpg",
		atmosScale: 1.05,
		colour: 0x44bbff,
		planet: null,
		orbit: null,
		atmosphere: null,
		group: new THREE.Group()},
	Mars : {
		name: "Mars",
		radius: EARTH_RADIUS*0.53,
		distance: (OFFSET_X-(OFFSET_PLANETS*4.5)),
		tilt: calculateTilt(25.19),
		rotationSpeed: 1.0260,
		rotationSpeedScale: 1,
		orbitInclination: calculateTilt(5.65),
		orbitSpeed: 1.8809,
		orbitSpeedScale: 1,
		textureMap: ASSET_URL_PREFIX+"mars/marsmap.jpg", 
		bumpMap: ASSET_URL_PREFIX+"mars/marsbump.jpg",
		colour: 0xffa555,
		planet: null,
		orbit: null,
		group: new THREE.Group()},
	Jupiter : {
		name: "Jupiter",
		radius: EARTH_RADIUS*2,
		distance: (OFFSET_X-(OFFSET_PLANETS*7.5)),
		tilt: calculateTilt(3.13),
		rotationSpeed: 0.4101,
		rotationSpeedScale: 1,
		orbitInclination: calculateTilt(6.09),
		orbitSpeed: 11.8618,
		orbitSpeedScale: 1,
		textureMap: ASSET_URL_PREFIX+"jupiter/jupitermap.jpg",
		bumpMap: ASSET_URL_PREFIX+"jupiter/jupitermap.jpg",
		colour: 0xefefac,
		planet: null,
		orbit: null,
		group: new THREE.Group()},
	Saturn : {
		name: "Saturn",
		radius: EARTH_RADIUS*1.5,
		distance: (OFFSET_X-(OFFSET_PLANETS*10.5)),
		tilt: calculateTilt(26.73),
		rotationSpeed: 0.4260,
		rotationSpeedScale: 1,
		orbitInclination: calculateTilt(5.51),
		orbitSpeed: 29.4767,
		orbitSpeedScale: 1,
		ringSize: 80,
		textureMap: ASSET_URL_PREFIX+"saturn/saturnmap.jpg",
		bumpMap: ASSET_URL_PREFIX+"saturn/saturnmap.jpg",
		ringMap: ASSET_URL_PREFIX+"saturn/saturnringmap.png",
		colour: 0x756f5e,
		planet: null,
		orbit: null,
		ring: null,
		group: new THREE.Group()},
	Uranus : {
		name: "Uranus",
		radius: EARTH_RADIUS*1.3,
		distance: (OFFSET_X-(OFFSET_PLANETS*13.5)),
		tilt: calculateTilt(82.23),
		rotationSpeed: 0.7183,
		rotationSpeedScale: 1,
		orbitInclination: calculateTilt(6.48),
		orbitSpeed: 84.0708,
		orbitSpeedScale: 1,
		ringSize: 40,
		textureMap: ASSET_URL_PREFIX+"uranus/uranusmap.jpg",
		ringMap: ASSET_URL_PREFIX+"uranus/uranusringmap.png",
		colour: 0xcedadc,
		planet: null,
		orbit: null,
		ring: null,
		group: new THREE.Group()},
	Neptune : {
		name: "Neptune",
		radius: EARTH_RADIUS*1.1/3,
		distance: (OFFSET_X-(OFFSET_PLANETS*16.5)),
		tilt: calculateTilt(28.32),
		rotationSpeed: 0.6713,
		rotationSpeedScale: 1,
		orbitInclination: calculateTilt(6.43),
		orbitSpeed: 164.8822,
		orbitSpeedScale: 1,
		ringSize: 10,
		textureMap: ASSET_URL_PREFIX+"neptune/neptunemap.jpg",
		ringMap: ASSET_URL_PREFIX+"neptune/neptuneringmap.png",
		colour: 0xbdd9de,
		planet: null,
		orbit: null,
		group: new THREE.Group()},
	Pluto : {
		name: "Pluto",
		radius: EARTH_RADIUS*1.1/2,
		distance: (OFFSET_X-(OFFSET_PLANETS*19.5)),
		tilt: calculateTilt(122.23),
		rotationSpeed: 6.38,
		rotationSpeedScale: 1,
		orbitInclination: calculateTilt(17.86),
		orbitSpeed: 247.7,
		orbitSpeedScale: 1,
		textureMap: ASSET_URL_PREFIX+"pluto/plutomap.jpg",
		bumpMap: ASSET_URL_PREFIX+"pluto/plutomap.jpg",
		colour: 0x69598e,
		planet: null,
		orbit: null,
		group: new THREE.Group()}
};

//Global object to hold planetary objects data
var planetaryObjects = {
	Moon : {
		name: "Moon",
		radius: EARTH_RADIUS*0.2726,
		distance: planetObjects.Earth.distance+75,
		tilt: calculateTilt(5.15),
		rotationSpeed: 0.75,
		rotationSpeedScale: 1,
		orbitInclination: planetObjects.Earth.orbitInclination,
		orbitSpeed: 0.07589,
		orbitSpeedScale: 1,
		textureMap: ASSET_URL_PREFIX+"moon/moonmap.jpg",
		bumpMap: ASSET_URL_PREFIX+"moon/moonmap.jpg",
		colour: 0x2a2a2a,
		planet: null,
		orbit: null,
		group: new THREE.Group()}
};

/*
 * letThereBeLight()
 *
 * Tries to form the universe, throws errors to console
 */
function letThereBeLight() {
	try {
		initScene();
		initPhysics();
		fillScene();
		setupGUI();
		addToDOM();
		animate();
		console.log("And there was light.");
	} catch(error) {
		console.log("Error forming the universe:");
		console.log(error);
	}
}

/*
 * initScene()
 *
 * Creates and setups the canvas, renderer, camera, and camera controls
 */
function initScene() {
	//Establish the canvas size and ratio
	var width = window.innerWidth-30;
	var height = window.innerHeight-30;
	var canvasRatio = width/height;
	//Create the webGL renderer for the scene
	renderer = new THREE.WebGLRenderer({
		antialias: true, 
		alpha: true,
		gammaInput: true,
		gammaOutput: true
	});
	renderer.setSize(width, height);
	renderer.setClearColor(0xAAAAAA, 1.0);
	//Create and set the camera's location
	camera = new THREE.PerspectiveCamera( 60, canvasRatio, 1, farClipping );
	camera.position.set( 5400, 3200, -2225);
	//Create and set the camera controls
	cameraControls = new THREE.OrbitControls(camera, renderer.domElement);
	cameraControls.target.set(200,-300,-125);
	//Set the last camera location variables
	lastCamX = camera.position.x;
	lastCamY = camera.position.y;
	lastCamZ = camera.position.z;
}

/*
 * initPhysics()
 *
 * Creates and configures the webGL physics engine
 *
 * Code copied from: https://github.com/mrdoob/three.js/blob/master/examples/webgl_physics_terrain.html
 */
function initPhysics() {
	collisionConfiguration = new Ammo.btDefaultCollisionConfiguration();
	dispatcher = new Ammo.btCollisionDispatcher( collisionConfiguration );
	broadphase = new Ammo.btDbvtBroadphase();
	solver = new Ammo.btSequentialImpulseConstraintSolver();
	physicsWorld = new Ammo.btDiscreteDynamicsWorld( dispatcher, broadphase, solver, collisionConfiguration );
	physicsWorld.setGravity( new Ammo.btVector3( 0, 0, 0 ) );
}

/*
 * fillScene()
 *
 * Adds all of the solar system pieces into the canvas
 */
function fillScene() {
	addLight();
	addSkybox();
	addSun();
	addPlanets();
	addMoon();
	addAsteroids();
	addStars();
}

/*
 * setupGUI()
 *
 * Creates the Dat.GUI and Stats.js overlays
 */
function setupGUI() {
	//Create GUI and Stats objects
	gui = new dat.GUI();
	stats = new Stats();
	gui.width = 300;
	//Create a global folder, add orbit and rotation speed
	var global = gui.addFolder('Global Scalars');
	global.add(this, 'orbitSpeed', 0, 500, 1);
	global.add(this, 'rotationSpeed', 0, 500, 1);
	//Create all the individual planet folders, add orbit and rotation scales
	var planets = gui.addFolder('Planets');
	for (planet in planetObjects) {
		var subfolder_planets = planets.addFolder(planetObjects[planet].name);
		subfolder_planets.add(planetObjects[planet], 'orbitSpeedScale', 0.01, 50, 0.01);
		subfolder_planets.add(planetObjects[planet], 'rotationSpeedScale', 0.01, 50, 0.01);
	}
	//Create all the planetary objects folders, add orbit and rotation scales
	var objects = gui.addFolder('Planetary Objects');
	for(objs in planetaryObjects) {
		var subfolder_objects = objects.addFolder(planetaryObjects[objs].name);
		subfolder_objects.add(planetaryObjects[objs], 'orbitSpeedScale', 0.01, 50, 0.01);
		subfolder_objects.add(planetaryObjects[objs], 'rotationSpeedScale', 0.01, 50, 0.01);
	}
	//Create the particles slider
	var particles = gui.add(this, 'maxParticles', 1, 10, 1);
    particles.onFinishChange(function (value) {
        for (emitter in proton.emitters) {
            emitter.rate = new Proton.Rate(new Proton.Span(1, value), new Proton.Span(.05, .1));
        }
        console.log("Max Particle Value changes to " + value);
    });
    //Create the FPS Monitor toggle
	var fps = gui.add(this, 'fpsMonitor');
	fps.onFinishChange(function(value) {
		if(fpsMonitor) {
			document.body.appendChild(stats.dom);
			console.log("FPS Monitor is enabled");
		} else {
			document.body.removeChild(stats.dom);
			console.log("FPS Monitor is disabled");
		}
	});
	//Show the panel, and if it is enabled, show it
	stats.showPanel(0);
	if(fpsMonitor) {
		document.body.appendChild(stats.dom);
	}
}

/*
 * addToDOM()
 *
 * Attaches the renderer to the HTML document
 */
function addToDOM() {
    var canvas = document.getElementById('canvas');
    canvas.appendChild(renderer.domElement);
}

/*
 * animate()
 *
 * Function called with update the animation frame for the renderer
 */
function animate() {
	//If the FPS monitor is enabled, begin the monitoring block
	if(fpsMonitor) {
		stats.begin();
	}
	//If the camera moved, update the lens flare
	if(lastCamX != camera.position.x || lastCamY != camera.position.y || lastCamZ != camera.position.z)
	{
		var distFromSun = SUN_RADIUS + 10;// + 10 to prevent flare from clipping into the sun
		var cameraMagnatude = Math.sqrt(Math.pow(camera.position.x-SUN_X, 2) + Math.pow(camera.position.y-SUN_Y, 2) + Math.pow(camera.position.z-SUN_Z, 2));
		var flareX = ((camera.position.x-SUN_X)/cameraMagnatude)*distFromSun;
		var flareY = ((camera.position.y-SUN_Y)/cameraMagnatude)*distFromSun;
		var flareZ = ((camera.position.z-SUN_Z)/cameraMagnatude)*distFromSun;
		lensFlare.position.set(flareX + SUN_X, flareY + SUN_Y, flareZ + SUN_Z);
		//Store the current camera position as the new old value
		lastCamX = camera.position.x;
		lastCamY = camera.position.y;
		lastCamZ = camera.position.z;
	}
	//Rotate and orbit the planets
	for (var planet in planetObjects) {
		planetObjects[planet].planet.rotation.y += rotationSpeed*0.001*planetObjects[planet].rotationSpeedScale/planetObjects[planet].rotationSpeed;
		planetObjects[planet].group.rotation.y += orbitSpeed*0.001*planetObjects[planet].orbitSpeedScale/planetObjects[planet].orbitSpeed;
		//If the planet has an atmosphere, rotate that as well
		if(planetObjects[planet].hasOwnProperty("atmosMap")) {
			planetObjects[planet].atmosphere.rotation.y += (rotationSpeed*0.001*planetObjects[planet].rotationSpeedScale/planetObjects[planet].rotationSpeed)/0.75;
		}
		//If the planet has a ring, rotate that as well
		if(planetObjects[planet].hasOwnProperty("ringMap")) {
			planetObjects[planet].ring.rotation.z -= rotationSpeed*0.001*planetObjects[planet].rotationSpeedScale/planetObjects[planet].rotationSpeed;
		}
	}
	//Rotate and orbit the moon
	delta2 -= orbitSpeed*0.001*planetaryObjects.Moon.orbitSpeedScale/planetaryObjects.Moon.rotationSpeed;
	var diff = Math.abs(planetObjects.Earth.distance - planetaryObjects.Moon.distance);
	planetaryObjects.Moon.planet.rotation.y += rotationSpeed*0.001*planetaryObjects.Moon.rotationSpeedScale/planetaryObjects.Moon.rotationSpeed;
    planetaryObjects.Moon.planet.position.x = planetObjects.Earth.planet.position.x + Math.cos(delta2)*diff;
    planetaryObjects.Moon.planet.position.z = planetObjects.Earth.planet.position.z + Math.sin(delta2)*diff;
	//Make the skybox follow the camera
	skybox.position.set(camera.position.x, camera.position.y, camera.position.z);
	//If FPS monitor is enabled, stop the monitoring block
	if(fpsMonitor) {
		stats.end();
	}
	//Request a new animation frame, and render
	window.requestAnimationFrame(animate);
	render();
}

/*
 * render()
 *
 * Function called to render the animation frame
 */
function render() {
	var delta = clock.getDelta();
	cameraControls.update(delta);
	updatePhysics();
    proton.update(delta);
	renderer.render(scene, camera);
}

/*
 * addLight()
 *
 * Creates and adds the global lighting (ambient and point lights)
 */
function addLight() {
	//Creates a scene with a subtle ambient light
	scene = new THREE.Scene();
	scene.add(new THREE.AmbientLight( 0xAAAAAA , 0.025));
	//Creates the Sun's light (point light), sets it to [0,0,0]
	sunLight = new THREE.PointLight(0x666666, 3, 0, 0);
	sunLight.position.set(SUN_X,SUN_Y,SUN_Z);
	scene.add( sunLight );
}

/*
 * addSkybox()
 *
 * Creates and adds the skybox for the entire scene
 */
function addSkybox() {
	//Create the cube that will be our skybox
	var skySize = farClipping/1.3;
	var geometry = new THREE.CubeGeometry(skySize,skySize,skySize);
	var cubeMaterials =
	[
		new THREE.MeshBasicMaterial( {
			map: new THREE.TextureLoader().load(ASSET_URL_PREFIX+"skybox/skyboxfront.jpg"), 
			side: THREE.BackSide}),
		new THREE.MeshBasicMaterial( {
			map: new THREE.TextureLoader().load(ASSET_URL_PREFIX+"skybox/skyboxback.jpg"), 
			side: THREE.BackSide}),
		new THREE.MeshBasicMaterial( {
			map: new THREE.TextureLoader().load(ASSET_URL_PREFIX+"skybox/skyboxup.jpg"), 
			side: THREE.BackSide}),
		new THREE.MeshBasicMaterial( {
			map: new THREE.TextureLoader().load(ASSET_URL_PREFIX+"skybox/skyboxdown.jpg"), 
			side: THREE.BackSide}),
		new THREE.MeshBasicMaterial( {
			map: new THREE.TextureLoader().load(ASSET_URL_PREFIX+"skybox/skyboxright.jpg"), 
			side: THREE.BackSide}),
		new THREE.MeshBasicMaterial( {
			map: new THREE.TextureLoader().load(ASSET_URL_PREFIX+"skybox/skyboxleft.jpg"), 
			side: THREE.BackSide})
	];
	cubeMaterials = new THREE.MeshFaceMaterial(cubeMaterials);
	skybox = new THREE.Mesh(geometry, cubeMaterials);
	//Add the skybox to the scene
	scene.add(skybox);
}

/*
 * addSun()
 * 
 * Creates the sun and the lightflare effect
 */
function addSun() {
	//Create the texture, body, and geometry of the sun
	var texture = new THREE.TextureLoader().load(ASSET_URL_PREFIX+"sun/sunmap.jpg");
	var bodyMaterial = new THREE.MeshBasicMaterial( {
		color: 0xFF0000,
		map: texture, 
		overdraw: 0.5,
		color: 0xffffff
	} );
	var geometry = new THREE.SphereGeometry(SUN_RADIUS, 32, 32);
	//Set the created sun to the constant sun [x,y,z] location
	var sun = new THREE.Mesh(geometry, bodyMaterial);
	sun.position.set(SUN_X, SUN_Y, SUN_Z);
	scene.add(sun);
	//Load the texture and set the flare variables
	var textureFlare = new THREE.TextureLoader().load(ASSET_URL_PREFIX+"lensflare/lensflare.png");
	var flareColor = new THREE.Color( 0xffffff);
	flareColor.setHSL(0.55, 0.9, 0.5 + 0.5);
	//Create the lens flare, and set it to the same position as the sun
	lensFlare = new THREE.LensFlare(textureFlare, 900, 0.0, THREE.AdditiveBlending, flareColor);
	lensFlare.position.copy(sun.position);
	//Add two more layers of texture to the lens flare
	var textureFlare = new THREE.TextureLoader().load(ASSET_URL_PREFIX+"lensflare/lensflare3.png");
	lensFlare.add( textureFlare, 900, 0.0, THREE.AdditiveBlending);
	var textureFlare = new THREE.TextureLoader().load(ASSET_URL_PREFIX+"lensflare/lensflare2.png");
	lensFlare.add( textureFlare, 70, 0.7, THREE.AdditiveBlending);
	lensFlare.add( textureFlare, 120, 0.9, THREE.AdditiveBlending);
	lensFlare.add( textureFlare, 70, 1.0, THREE.AdditiveBlending);
	//Add the lens flare to the scene
	scene.add( lensFlare );
	//Add the particle effect to the sun
	addProton();
}

/*
 * addPlanets()
 * 
 * Creates and adds every planet in the planetObjects object array
 */
function addPlanets() {
	//Loop through all the planets in the array and "assemble" them
	for (var planet in planetObjects) {
		planetObjects[planet].planet = createPlanetMesh(planetObjects[planet]);
		planetObjects[planet].planet.position.set(planetObjects[planet].distance, OFFSET_Y, 0);
		planetObjects[planet].planet.rotation.set(planetObjects[planet].tilt,0,0);
		planetObjects[planet].group.add(planetObjects[planet].planet);
		//If the planet has an atmosphere, add it
		if(planetObjects[planet].hasOwnProperty("atmosMap")) {
			addAtmosphere(planetObjects[planet]);
		}
		//If the planet has rings, add them
		if(planetObjects[planet].hasOwnProperty("ringMap")) {
			addRing(planetObjects[planet]);
		}
		//Create the orbits for the planet
		addOrbit(planetObjects[planet]);
		//Add the entire planet group (planet, orbit, rings, etc.) to the scene
		scene.add(planetObjects[planet].group);
	}
}

/*
 * addMoon()
 *
 * Creates and adds the moon to orbit the Earth
 */
function addMoon() {
	//Create the Moon
	planetaryObjects.Moon.planet = createPlanetMesh(planetaryObjects.Moon);
	planetaryObjects.Moon.planet.position.set(planetaryObjects.Moon.distance, OFFSET_Y, 0);
	planetaryObjects.Moon.group.add(planetaryObjects.Moon.planet);
	//Create the orbit, around the moon
	var orbit = new THREE.CircleGeometry((Math.abs(planetObjects.Earth.distance-planetaryObjects.Moon.distance)), 100);
	orbit.vertices.shift();
	planetaryObjects.Moon.orbit = new THREE.Line(
		orbit,
		new THREE.LineDashedMaterial({
			color: planetaryObjects.Moon.colour,
			transparent: true,
			opacity: 0.50
		})
	);
	//Set the Moon's rotation and position
	planetaryObjects.Moon.group.add(planetaryObjects.Moon.orbit);
	planetaryObjects.Moon.orbit.rotation.x = Math.PI/2;
	planetaryObjects.Moon.orbit.position.copy(planetObjects.Earth.planet.position);
	//Add the Moon back to the Earth's group for rotating around the sun
	planetObjects.Earth.group.add(planetaryObjects.Moon.group);
}

/*
 * addAsteroids()
 *
 * Creates and adds asteroids into the solar system
 */
function addAsteroids() {
	//All variables needed for asteroid force calculations
	var radiusLower = 5;
	var radiusMaxAdd = 3;
	var aRadius;
	var moon;
	var aMass = 40;
	var aOffset = OFFSET_X-(OFFSET_PLANETS*3)-170;
	var aOffsetMaxAdd = -60;
	var aOffsetTemp;
	var initX, initZ;
	var aForce = -500;
	var moonProperties;
	var initForce;
	//Loop for each asteroid spawning
	for (var i = 0; i < NUM_OF_ASTEROIDS; i++) {
		aOffsetTemp = aOffset + Math.random()*aOffsetMaxAdd;
		aRadius = radiusLower + Math.random()*radiusMaxAdd;
		//Use the same texture as the moon for ease
		moonProperties = { 
			radius: aRadius,
			textureMap : ASSET_URL_PREFIX+"moon/moonmap.jpg",
			bumpMap: ASSET_URL_PREFIX+"moon/moonmap.jpg",
		};
		moon = createPlanetMesh(moonProperties);
		//Spawn the asteroids with force evenly spread and forced based on the number of asteroids 
		initX = aOffsetTemp*Math.cos(i*(2*Math.PI)/NUM_OF_ASTEROIDS);
		initZ = aOffsetTemp*Math.sin(i*(2*Math.PI)/NUM_OF_ASTEROIDS);
		initForce = new Ammo.btVector3( aForce*Math.cos((i - 0.25*NUM_OF_ASTEROIDS)*(2*Math.PI)/NUM_OF_ASTEROIDS) , 0 , aForce*Math.sin((i - 0.25*NUM_OF_ASTEROIDS)*(2*Math.PI)/NUM_OF_ASTEROIDS) );
		spawnPlanet(initX, OFFSET_Y, initZ, aMass, aRadius, moon.clone(), initForce);
	}
}

/*
 * addStars()
 *
 * Creates and adds stars (points) into the solar system
 */
 function addStars() {
 	var numStars = farClipping * Math.random();
 	var starsGeometry = new THREE.Geometry();
 	var maxPosition = farClipping;

	//Make numStars amount of stars
	for (var i=0; i<numStars; i++) {
		//Create a vector point in a random location
		var star = new THREE.Vector3();
		star.x = THREE.Math.randFloatSpread(maxPosition);
		star.y = THREE.Math.randFloatSpread(maxPosition);
		star.z = THREE.Math.randFloatSpread(maxPosition);
		starsGeometry.vertices.push(star);
		//For the first forty vector points, give them a pointlight
		if(i < 40) {
			var intensity = Math.random()*0.001;
			var starLight = new THREE.PointLight(0xffffff, intensity, 0, 2);
			starLight.position.set(star.x, star.y, star.z);
			scene.add(starLight);
		}
	}
	//Assemble the starfield and add it to the scene
	var starsMaterial = new THREE.PointsMaterial({color: 0x888888});
	var starField = new THREE.Points(starsGeometry, starsMaterial);
	scene.add(starField);
}

/*
 * addRing(planet)
 * 		- Accepts a planet object
 *
 * Loads in and adds a ring to a planet
 */
function addRing(planet) {
	//Loads in the texture and makes the ring material
	var texture = new THREE.TextureLoader().load(planet.ringMap);
	var ringMaterial = new THREE.MeshBasicMaterial({
		side: THREE.DoubleSide,
		map: texture, 
		transparent: true,
		opacity: 0.50
	});
	//Sets the inner and outer ring radius
	var innerRadius = planet.planet.geometry.parameters.radius + 20;
	var outerRadius = innerRadius + planet.ringSize;
	var geometry = new THREE.RingGeometry(innerRadius, outerRadius, 500);
	//Creates the ring mesh, set it's position and rotation and add to the planet's group
	planet.ring = new THREE.Mesh(geometry, ringMaterial);
	planet.ring.position.set(planet.planet.position.x, planet.planet.position.y, planet.planet.position.z);
	planet.ring.rotation.set((Math.PI/2+planet.tilt), 0, 0);
	planet.group.add(planet.ring);
}

/*
 * addOrbit(planet)
 * 		- Accepts a planet object
 *
 * Create a circleGeometry to be the orbit for the planet
 */
function addOrbit(planet) {
	//Create a circle with a translucent dashed line geometry for the orbit
	var geometry = new THREE.CircleGeometry((Math.abs(planet.distance)+SUN_X), 100);
	geometry.vertices.shift();
	planet.orbit = new THREE.Line(
		geometry,
		new THREE.LineDashedMaterial({
			color: planet.colour,
			transparent: true,
			opacity: 0.50
		})
	);
	//Sets the orbit's rotation, position, and add it to the planet's group
	planet.orbit.rotation.x = Math.PI*0.5;
	planet.orbit.position.set(SUN_X,SUN_Y,SUN_Z);
	planet.group.add(planet.orbit);
	//Tilt the orbit off the planet's inclination
	planet.group.rotation.x = planet.orbitInclination;
}

/*
 * addAtmosphere(planet)
 * 		- Accepts a planet object
 *
 * Load and add an atmosphere for the supplied planet
 */
function addAtmosphere(planet) {
	//Load in the atmosphere texture
	var texture = new THREE.TextureLoader().load(planet.atmosMap);
	var material = new THREE.MeshBasicMaterial(
        {
        	map: texture,
        	opacity: 0.50,
        	transparent: true,
        	side: THREE.FrontSide,
			reflectivity: 0,
			blending: THREE.AdditiveBlending
        });
	//Create the mesh, and scale it slightly bigger than the planet itself
	planet.atmosphere = new THREE.Mesh(planet.planet.geometry.clone(), material);
    planet.atmosphere.scale.multiplyScalar(planet.atmosScale);
    //Position and rotation the planet according to the planet's specifications
    planet.atmosphere.position.set(planet.planet.position.x, planet.planet.position.y, planet.planet.position.z);
    planet.atmosphere.rotation.set(planet.tilt,0,0);
    //Add the atmosphere to the planet's group
    planet.group.add(planet.atmosphere);
}

/*
 * addProton()
 *
 * Adds all of the specular effects to the sun
 */
function addProton() {
	//Create the proton ojbject
    proton = new Proton();
    //Add emitters for showing the particle "pops"
    proton.addEmitter(createEmitter(0, 750, 0));
    proton.addEmitter(createEmitter(0, -175, 0));
    for (var i=0; i<360; i+=10){
        var x = (SUN_RADIUS - 25 )* Math.cos(i*Math.PI/180) * Math.sin(i*Math.PI/360);
        var y = (SUN_RADIUS - 25 )* Math.sin(i*Math.PI/180) * Math.sin(i*Math.PI/360);
        var z = (SUN_RADIUS - 25 ) * Math.cos(i*Math.PI/360);
        proton.addEmitter(createEmitter(x,OFFSET_Y + y,z));
        proton.addEmitter(createEmitter(-x,OFFSET_Y + y,-z));
        proton.addEmitter(createEmitter(-x,OFFSET_Y + y,z));
        proton.addEmitter(createEmitter(x,OFFSET_Y + y,-z));
        proton.addEmitter(createEmitter(x,OFFSET_Y - y,z));
        proton.addEmitter(createEmitter(-x,OFFSET_Y - y,-z));
        proton.addEmitter(createEmitter(-x,OFFSET_Y - y,z));
        proton.addEmitter(createEmitter(x,OFFSET_Y - y,-z));
    }
    proton.addRender(new Proton.SpriteRender(scene));
}

/*
 * addEmitter(x,y,z)
 * 		- Accepts [x,y,z] cooridinates for the emitter
 *
 * Creates the emitters that create the particle effects for THREE.Proton
 */
function createEmitter(x,y,z) {
	//Create the emitter object, and set it's parameters to look like solar flares
    emitter = new Proton.Emitter();
    emitter.rate = new Proton.Rate(new Proton.Span(1, maxParticles), new Proton.Span(.05, 5));
    emitter.addInitialize(new Proton.Body(createSprite()));
    emitter.addInitialize(new Proton.Mass(10));
    emitter.addInitialize(new Proton.Life(1, 2));
    emitter.addInitialize(new Proton.Position(new Proton.SphereZone(0,0,0,50)));
    emitter.addInitialize(new Proton.V(new Proton.Span(500, 800), new Proton.Vector3D(0, 0, 0), 30));
    emitter.addBehaviour(new Proton.RandomDrift(1, 1, 1, .05));
    emitter.addBehaviour(new Proton.Scale(new Proton.Span(2, 3.5), 0));
    emitter.addBehaviour(new Proton.Color('#FF0000', ['#ffff11', '#ffff11'], Infinity, Proton.easeOutSine));
    emitter.p.x = x;
    emitter.p.y = y;
    emitter.p.z = z;
    emitter.emit();
    //Return the emitter that was created
    return emitter;
}

/*
 * createSprite()
 *
 * Creates the sprite used by emitters in THREE.Proton
 */
function createSprite() {
	//Loads in the texture, and returns the Sprite object made from it
    var map = new THREE.TextureLoader().load(ASSET_URL_PREFIX+"sun/sundot.png");
    var material = new THREE.SpriteMaterial({
        map: map,
        color: 0xff0000,
        blending: THREE.AdditiveBlending,
        fog: true
    });
    return new THREE.Sprite(material);
}

/*
 * calculateTilt(degrees)
 * 		- Accepts angle in degrees
 *
 *
 * Converts an angle in degrees to radians 
 */
function calculateTilt(degrees) {
 	return (degrees*Math.PI/180);
}

/*
 * createPlanetMesh(planet)
 * 		- Accepts a planet object for which to create the mesh material
 *
 * Loads in the planet textures and creates the planet mesh
 */
function createPlanetMesh(planet)
{
	//Loads in the texture, creates the material
	var texture = new THREE.TextureLoader().load(planet.textureMap);
	var bodyMaterial = new THREE.MeshPhongMaterial({
			shininess: 25,
			map: texture, 
			overdraw: 0.5
		});
	//If the planet has a bumpmap, load that in as well
	if(planet.hasOwnProperty("bumpMap")) {
		var bumpmap = new THREE.TextureLoader().load(planet.bumpMap);
		bodyMaterial.bumpMap = bumpmap;
		bodyMaterial.bumpScale = 12;
	}
	//Create the planet based on it's radius, and return the Mesh object
	var geometry = new THREE.SphereGeometry(planet.radius, 32, 32);
	return sphere = new THREE.Mesh(geometry, bodyMaterial);
}

/*
 * spawnPlanet(x, y, z, mass, radius, threeObject, initForce)
 * 		- Accepts [x,y,z] location, the mass of the object, the radius of the object, the Mesh object, and a force to apply onto the object
 * Adds all of the specular effects to the sun
 */
function spawnPlanet(x, y, z, mass, radius, threeObject, initForce) {
	var shape = new Ammo.btSphereShape( radius );
	shape.setMargin( margin );
	threeObject.position.set(x, y, z);
	updatePhysics(  );
	var localInertia = new Ammo.btVector3( 0, 0, 0 );
	shape.calculateLocalInertia( mass, localInertia );
	var transform = new Ammo.btTransform();
	transform.setIdentity();
	var pos = threeObject.position;
	transform.setOrigin( new Ammo.btVector3( pos.x, pos.y, pos.z ) );
	var motionState = new Ammo.btDefaultMotionState( transform );
	var rbInfo = new Ammo.btRigidBodyConstructionInfo( mass, motionState, shape, localInertia );
	var body = new Ammo.btRigidBody( rbInfo );
	threeObject.userData.physicsBody = body;
	scene.add( threeObject );
	dynamicObjects.push( threeObject );
	dynamicObjectsMass.push(mass);
	physicsWorld.addRigidBody( body );
	if (initForce != null) {
		var objPhys = threeObject.userData.physicsBody;
		objPhys.setLinearVelocity(initForce);
	}
}

/*
 * updatePhysics()
 *
 * Updates the physics engine and apply the forces on objects that are effected
 */
function updatePhysics() {
	//Step the physics simulation
	physicsWorld.stepSimulation( 0.007, 10 );
	//For each dynamic physics-based object, step through and perform physics operations
	for ( var i = 0, il = dynamicObjects.length; i < il; i++ ) {
		var objThree = dynamicObjects[ i ];
		var objPhys = objThree.userData.physicsBody;
		var ms = objPhys.getMotionState();
		if (ms) {
			ms.getWorldTransform( transformAux1);
			var p = transformAux1.getOrigin();
			var q = transformAux1.getRotation();
			objThree.position.set( p.x(), p.y(), p.z());
			objThree.quaternion.set( q.x(), q.y(), q.z(), q.w());
		}
	}
	//Applies the forces
	applyForces();
}

/*
 * applyForces()
 *
 * Applies the forces on all of the objects
 */
function applyForces() {
	var forceIsh;
	var vecOut = new Ammo.btVector3(0,0,0);
	var vecOut2 = new Ammo.btVector3(0,0,0);
	var xDist, yDist, zDist;
	var distanceCube;
	var objThree;
	var objPhys;
	var sunMass = 70000;
	var l,j,k;// final vector
	for (var i = 0; i<dynamicObjects.length; i++)
	{
		xDist = dynamicObjects[i].position.x-SUN_X;
		yDist = dynamicObjects[i].position.y-SUN_Y;
		zDist = dynamicObjects[i].position.z-SUN_Z;
		distanceCube = Math.pow (Math.sqrt( Math.pow(xDist, 2) + Math.pow(yDist, 2) + Math.pow(zDist, 2)), 3);
		if (distanceCube > SUN_RADIUS) {
			//Divided by distance cubed instead of squared so that directional vector can
			//Have magnatude of force
			forceIsh = ((sunMass*dynamicObjectsMass[i])/distanceCube) * GRAVITY_CONSTANT *-1;
			objThree = dynamicObjects[ i ];
			objPhys = objThree.userData.physicsBody;	
			//Magnitude is force
			l = objPhys.getLinearVelocity().x()+xDist*forceIsh;
			j = objPhys.getLinearVelocity().y()+yDist*forceIsh;
			k = objPhys.getLinearVelocity().z()+zDist*forceIsh;
			vecOut.setValue(l, j, k);
			objPhys.setLinearVelocity(vecOut);
		}
	}
}

/*
 * eventListener('keypress')
 *
 * Listens for keypresses, and reacts if the word "bruce" is typed
 */
document.body.addEventListener('keypress',function(ev) {
	//Hold the key that was pressed
	input += String.fromCharCode(ev.which);
	//If the input is equal to bruce, continue
	if(input.toLowerCase().includes("bruce") == true) {
		input = "";
		//Create the planet
		var texture = new THREE.TextureLoader().load(ASSET_URL_PREFIX+"pokemon/pokeballmap.jpg");
		var bodyMaterial = new THREE.MeshPhongMaterial({
				shininess: 25,
				map: texture, 
				overdraw: 0.5
			});
		var geometry = new THREE.SphereGeometry(100, 32, 32);
		//Create the force on the planet
		var force = -200;
		var cameraMagnatude = Math.sqrt(Math.pow(camera.position.x-SUN_X, 2) + Math.pow(camera.position.y-SUN_Y, 2) + Math.pow(camera.position.z-SUN_Z, 2));
		var bruceX = ((camera.position.x-SUN_X)/cameraMagnatude)*force;
		var bruceY = ((camera.position.y-SUN_Y)/cameraMagnatude)*force;
		var bruceZ = ((camera.position.z-SUN_Z)/cameraMagnatude)*force;
		force = new Ammo.btVector3(bruceX,bruceY,bruceZ);
		//Launch the pokeball into space
		spawnPlanet(camera.position.x, camera.position.y, camera.position.z, 50, 100, new THREE.Mesh(geometry, bodyMaterial), force);
	}
});

//Start the universe
letThereBeLight();