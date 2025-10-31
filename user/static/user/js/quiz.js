document.addEventListener('DOMContentLoaded', function() {
  const fill  = document.querySelector('.quiz-progress-fill');
  const track = document.querySelector('.quiz-progress-track');

  if (!fill || !track) {
    console.warn("[Quiz] Missing elements", { fill: !!fill, track: !!track });
    return;
  }

  // Read percentage from data attribute rendered by Django
  const raw = fill.getAttribute('data-width');
  const value = Math.max(0, Math.min(100, parseFloat(raw)));
  console.log("[Quiz] data-width:", raw, "→ clamped:", value);

  // Defensive: neutralize common framework collisions, JS-only
  track.style.setProperty('display', 'block', 'important');
  track.style.setProperty('position', 'relative', 'important');
  track.style.setProperty('overflow', 'hidden', 'important');

  fill.style.setProperty('display', 'block', 'important');
  fill.style.setProperty('flex', '0 0 auto', 'important');
  fill.style.setProperty('height', '100%', 'important');
  fill.style.setProperty('transform', 'none', 'important');
  fill.style.setProperty('width', value + '%', 'important');

  // Accessibility
  fill.setAttribute('aria-valuenow', String(value));

  // Debug readback
  const csTrack = getComputedStyle(track);
  const csFill  = getComputedStyle(fill);
  console.log("[Quiz] track width:", csTrack.width,
              "| fill width:", csFill.width,
              "| style:", fill.getAttribute('style'));

  // If computed is still 0, dump ancestors so we can spot a culprit quickly
  if (parseFloat(csFill.width) === 0) {
    console.warn("[Quiz] Computed width still 0 → auditing ancestors…");
    let n = fill;
    while (n) {
      const c = getComputedStyle(n);
      console.log(
        n.tagName.toLowerCase(),
        n.className || '(no class)',
        'offsetWidth=', n.offsetWidth,
        'display=', c.display,
        'visibility=', c.visibility,
        'position=', c.position
      );
      n = n.parentElement;
    }
  }
});
