document.addEventListener('DOMContentLoaded', () => {
  const fill = document.querySelector('.quiz-progress-fill');
  if (!fill) return;

  // Read % from Django-rendered data attribute
  const pct = Number(fill.dataset.width);
  if (!Number.isFinite(pct)) return;

  // Clamp and apply
  fill.style.width = Math.max(0, Math.min(100, pct)) + '%';
});
