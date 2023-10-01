importPackage(Packages.com.sk89q.worldedit);
importPackage(Packages.com.sk89q.worldedit.math);
importPackage(Packages.com.sk89q.worldedit.blocks);

var blocks = context.remember();
var session = context.getSession();
var player = context.getPlayer();
var region = session.getRegionSelector(player.getWorld()).getRegion();

/*
Select area
Place stone at all bits

- will clear stone
- report positions
*/

// blocks.setBlock(region.getMinimumPoint(), context.getBlock("green_wool"));
// blocks.setBlock(region.getMaximumPoint(), context.getBlock("red_wool"));

let areaSize = {
    x: region.getMaximumPoint().getX() - region.getMinimumPoint().getX(),
    y: region.getMaximumPoint().getY() - region.getMinimumPoint().getY(),
    z: region.getMaximumPoint().getZ() - region.getMinimumPoint().getZ(),
    getSize: function () {return Math.max(this.x, this.y, this.z) + 1}
};
let numberOfDimensions = (areaSize.x != 0) + (areaSize.y != 0) + (areaSize.z != 0);

if (numberOfDimensions != 1) {
    context.error("Please select a line!");
    context.exit();
}

function getBlock([x,y,z]) {
    return blocks.getBlock(BlockVector3.at(x,y,z));
}

function setBlock([x,y,z], block) {
    blocks.setBlock(BlockVector3.at(x,y,z), context.getBlock(block));
}


const BIT_MARKER = "minecraft:black_wool";
const MSB_MARKER = "minecraft:red_wool";


// find bits & MSB
let bits = [];
// will contain LSB ... MSB style
let found_msb = false;
for (let x = region.getMinimumPoint().getX(); x<=region.getMaximumPoint().getX(); x++) {
    for (let y = region.getMinimumPoint().getY(); y<=region.getMaximumPoint().getY(); y++) {
        for (let z = region.getMinimumPoint().getZ(); z<=region.getMaximumPoint().getZ(); z++) {
            let bl = getBlock([x,y,z]);
            if (bl == BIT_MARKER || bl == MSB_MARKER) {
                bits.push([x,y,z]);
            }
            if (bl == MSB_MARKER) {
                if (found_msb) {
                    context.error("Please only mark one MSB with infested chiseled!");
                    context.exit();
                }
                found_msb = true;
            }
        }
    }
}
if (getBlock(bits[0]) == MSB_MARKER) {
    bits.reverse();
}

if (bits.length != 8 || !found_msb) {
    context.error("Please add 8 bits of infested bricks and mark MSB with infested chiseled!!");
    context.exit();
}

if (getBlock(bits[bits.length - 1]) != MSB_MARKER) {
    context.error("Warning: messed up MSB placement somehow");
    context.exit();
}

const posDiff = ([x1,y1,z1], [x2, y2, z2]) => [x1-x2, y1-y2, z1-z2]; 
const posAdd = ([x1,y1,z1], [x2, y2, z2]) => [x1+x2, y1+y2, z1+z2];
const arrayEq = (a, b) => a.length === b.length && a.every((e, i) => e === b[i]);

for (let i = 0; i<6; i++) {
    if (!arrayEq(posDiff(bits[i], bits[i+1]), posDiff(bits[i+1], bits[i+2]))) {
        context.error("Please place bits evenly!");
        context.exit();
    }
}

// context.checkArgs(1, 1, "<num>");

const shiftDir = "+X";

function setPattern(num, shift, idx) {
    // currently only adds -X
    if (shiftDir != "+X") {
        context.error("Not implemented shift dir!");
        context.exit();
    }
    for (let i = 0; i<8; i++) {
        let pos = posAdd(bits[i], [-idx*shift, 0, 0]);
        if (getBlock(pos) == "minecraft:air") {
            setBlock(pos, "stone");
        }
        if ((num>>i) & 1) {
            setBlock(posAdd(pos, [0,1,0]), 'redstone_torch');
        } else {
            setBlock(posAdd(pos, [0,1,0]), 'air');
        }
    }
}

// const data = [249, 73, 64];
const data = [160, 11, 246, 30, 31, 109, 144, 110, 79, 127, 182, 135, 62, 90, 239, 90, 162, 34, 85, 122, 253, 130, 17, 209, 139, 109, 110, 250, 240, 95, 210, 93, 136, 33];
for (let i = 0; i<data.length; i++) {
    setPattern(data[i], areaSize.getSize(), i);
    context.print("Set bit " + i + " to " + data[i]);
}
// setPattern(parseInt(argv[1], 16));
