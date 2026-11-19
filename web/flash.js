// Writes a UF2 onto hendl from the browser. Put the module into its bootloader
// (hold BOOT while connecting USB); it shows up as a drive called RP2350, and
// the File System Access API writes the firmware onto it.

const FIRMWARE_URL = 'https://github.com/wgd-modular/hendl-firmware/releases/latest/download/hendl.uf2';
const UF2_MAGIC0 = 0x0a324655;
const UF2_MAGIC1 = 0x9e5d5157;

const el = id => document.getElementById(id);
const status = el('status');
const bar = el('bar');
const fill = el('fill');

function say(message, kind = '') {
  status.textContent = message;
  status.className = 'status' + (kind ? ' ' + kind : '');
}

function looksLikeUf2(bytes) {
  if (bytes.length < 512 || bytes.length % 512) return false;
  const view = new DataView(bytes.buffer, bytes.byteOffset, 8);
  return view.getUint32(0, true) === UF2_MAGIC0 && view.getUint32(4, true) === UF2_MAGIC1;
}

// INFO_UF2.TXT on the drive tells us which board's bootloader this is.
async function checkDrive(dir) {
  try {
    const file = await (await dir.getFileHandle('INFO_UF2.TXT')).getFile();
    const info = await file.text();
    if (/RP2350/i.test(info)) return { ok: true };
    if (/RP2040|RPI-RP2/i.test(info)) {
      return { ok: false, reason: 'That is an RP2040 board — hendl runs on the RP2350 (Raspberry Pi Pico 2).' };
    }
    return { ok: false, reason: `“${dir.name}” is a bootloader drive for a different board.` };
  } catch {
    return { ok: false, reason: `“${dir.name}” is not the bootloader drive. Hold BOOT, reconnect, and pick the drive named RP2350.` };
  }
}

async function writeUf2(dir, bytes) {
  const handle = await dir.getFileHandle('hendl.uf2', { create: true });
  const writable = await handle.createWritable();
  const chunk = 64 * 1024;
  bar.hidden = false;
  for (let offset = 0; offset < bytes.length; offset += chunk) {
    await writable.write(bytes.subarray(offset, offset + chunk));
    fill.style.width = Math.min(100, ((offset + chunk) / bytes.length) * 100) + '%';
  }
  // hendl reboots the moment the last block lands, which can pull the drive
  // away before the browser finishes closing the file — that is success.
  try { await writable.close(); } catch { /* expected */ }
}

async function firmwareBytes() {
  const local = el('file').files[0];
  if (local) return new Uint8Array(await local.arrayBuffer());
  say('Fetching the latest firmware…');
  const res = await fetch(FIRMWARE_URL, { redirect: 'follow' });
  if (!res.ok) {
    throw new Error(`Could not download the firmware (${res.status}). If no release is out yet, pick a local .uf2 below.`);
  }
  return new Uint8Array(await res.arrayBuffer());
}

async function flash() {
  el('flash').disabled = true;
  try {
    const bytes = await firmwareBytes();
    if (!looksLikeUf2(bytes)) throw new Error('That file is not a UF2.');

    say('Pick the drive named RP2350…');
    const dir = await window.showDirectoryPicker({ id: 'hendl-rp2350', mode: 'readwrite' });
    if ((await dir.requestPermission({ mode: 'readwrite' })) !== 'granted') {
      throw new Error('Writing to the drive was not allowed.');
    }
    const check = await checkDrive(dir);
    if (!check.ok) throw new Error(check.reason);

    say('Writing firmware…');
    await writeUf2(dir, bytes);
    fill.style.width = '100%';
    say('Done — hendl rebooted into the new firmware.', 'ok');
  } catch (err) {
    bar.hidden = true;
    if (err.name === 'AbortError') say('Cancelled.');
    else say(err.message || String(err), 'err');
  } finally {
    el('flash').disabled = false;
  }
}

if ('showDirectoryPicker' in window) {
  el('flash').addEventListener('click', flash);
} else {
  el('unsupported').hidden = false;
  el('flash').disabled = true;
  el('download').href = FIRMWARE_URL;
}
