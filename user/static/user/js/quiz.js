document.addEventListener('DOMContentLoaded', () => {
  // Modal + layout elements
  const quizModal = document.getElementById('quizModal');
  const startQuizBtn = document.getElementById('startQuiz');
  const browseSuitcasesBtn = document.getElementById('browseSuitcases');
  const closeQuizBtn = document.querySelector('.close');
  const suitcasesList = document.getElementById('suitcasesList');
  const welcomeSection = document.getElementById('welcomeSection');
  const recommendationsSection = document.getElementById('recommendationsSection');
  const appShell = document.querySelector('.notuser');
  const retakeQuizBtn = document.getElementById('retakeQuiz');
  const showAllSuitcasesBtn = document.getElementById('showAllSuitcases');

  // Quiz elements
  const quizSteps = document.querySelectorAll('.quiz-step');
  const prevBtn = document.getElementById('prevBtn');
  const nextBtn = document.getElementById('nextBtn');
  const submitBtn = document.getElementById('submitQuiz');
  const stepIndicator = document.getElementById('stepIndicator');

  let currentStep = 0;
  const selectedCategoriesByStep = {}; // { stepId: [categories...] }

  // ----- Event wiring -----
  startQuizBtn?.addEventListener('click', () => {
    showModal(true);
    resetQuiz();
  });

  browseSuitcasesBtn?.addEventListener('click', () => {
    showSection('suitcases');
  });

  closeQuizBtn?.addEventListener('click', () => {
    showModal(false);
  });

  window.addEventListener('click', (evt) => {
    if (evt.target === quizModal) {
      showModal(false);
    }
  });

  retakeQuizBtn?.addEventListener('click', () => {
    showSection('welcome');

    // Best-effort clear of any stored recommendations server-side
    fetch('/users/clear-recommendations/', {
      method: 'POST',
      headers: {
        'X-CSRFToken': getCSRFToken(),
        'Content-Type': 'application/json',
      },
    }).catch(() => { /* ignore */ });
  });

  showAllSuitcasesBtn?.addEventListener('click', () => {
    showSection('suitcases');
  });

  prevBtn?.addEventListener('click', () => {
    if (currentStep > 0) {
      currentStep--;
      updateStep();
    }
  });

  nextBtn?.addEventListener('click', () => {
    const stepId = quizSteps[currentStep]?.id;
    if (!stepId || !selectedCategoriesByStep[stepId]) {
      alert('Please select an answer before continuing.');
      return;
    }
    if (currentStep < quizSteps.length - 1) {
      currentStep++;
      updateStep();
    }
  });

  submitBtn?.addEventListener('click', () => {
    const stepId = quizSteps[currentStep]?.id;
    if (!stepId || !selectedCategoriesByStep[stepId]) {
      alert('Please select an answer before submitting.');
      return;
    }
    submitQuiz();
  });

  // Option selection (event delegation)
  document.addEventListener('click', (evt) => {
    const btn = evt.target.closest('.option-btn');
    if (!btn) return;

    const questionEl = btn.closest('.quiz-step');
    if (!questionEl) return;

    // Visual selection
    questionEl.querySelectorAll('.option-btn').forEach(b => b.classList.remove('selected'));
    btn.classList.add('selected');

    // Store categories
    const stepId = questionEl.id;
    const csv = btn.dataset.categories || '';
    selectedCategoriesByStep[stepId] = csv ? csv.split(',') : [];

    updateButtonStates();
  });

  // ----- UI helpers -----
  function showModal(visible) {
    if (!quizModal || !appShell) return;
    quizModal.style.display = visible ? 'block' : 'none';
    appShell.style.display = visible ? 'none' : 'flex';
  }

  function showSection(kind) {
    // kind: 'welcome' | 'recommendations' | 'suitcases'
    if (welcomeSection) welcomeSection.style.display = kind === 'welcome' ? 'block' : 'none';
    if (recommendationsSection) recommendationsSection.style.display = kind === 'recommendations' ? 'block' : 'none';
    if (suitcasesList) suitcasesList.style.display = kind === 'suitcases' ? 'block' : 'none';
  }

  function updateStep() {
    quizSteps.forEach(step => step.classList.remove('active'));
    quizSteps[currentStep]?.classList.add('active');
    if (stepIndicator) stepIndicator.textContent = `Question ${currentStep + 1} of ${quizSteps.length}`;
    updateButtonStates();
  }

  function updateButtonStates() {
    const stepId = quizSteps[currentStep]?.id || null;
    const answered = stepId ? Boolean(selectedCategoriesByStep[stepId]) : false;

    if (prevBtn) prevBtn.disabled = currentStep === 0;

    const onLast = currentStep === quizSteps.length - 1;

    if (onLast) {
      if (nextBtn) nextBtn.style.display = 'none';
      if (submitBtn) {
        submitBtn.style.display = 'inline-block';
        submitBtn.disabled = !answered;
      }
    } else {
      if (nextBtn) {
        nextBtn.style.display = 'inline-block';
        nextBtn.disabled = !answered;
      }
      if (submitBtn) submitBtn.style.display = 'none';
    }
  }

  function resetQuiz() {
    currentStep = 0;
    for (const key in selectedCategoriesByStep) delete selectedCategoriesByStep[key];
    document.querySelectorAll('.option-btn').forEach(btn => btn.classList.remove('selected'));
    updateStep();
  }

  function submitQuiz() {
    const chosen = [];
    Object.values(selectedCategoriesByStep).forEach(categories => {
      if (Array.isArray(categories)) chosen.push(...categories);
    });

    // Loading state
    if (submitBtn) {
      submitBtn.disabled = true;
      submitBtn.textContent = 'Finding your perfect match...';
    }

    fetch(window.location.href, {
      method: 'POST',
      headers: {
        'X-Requested-With': 'XMLHttpRequest',
        'X-CSRFToken': getCSRFToken(),
        'Content-Type': 'application/json',
      },
      body: JSON.stringify({ categories: chosen }),
    })
      .then(r => r.json())
      .then(data => {
        if (data.success) {
          showModal(false);
          showSection('recommendations');
          updateRecommendationsDisplay(data.recommendations);
        } else {
          alert('Error calculating recommendations: ' + (data.error || 'Unknown error'));
          resetSubmitButton();
        }
      })
      .catch(() => {
        alert('Error calculating recommendations');
        resetSubmitButton();
      });
  }

  function updateRecommendationsDisplay(recommendations) {
    const container = document.getElementById('recommendationsContainer');
    if (!container) return;

    if (!Array.isArray(recommendations) || recommendations.length === 0) {
      container.innerHTML = `
        <div class="no-recommendations">
          <h3>No perfect matches found</h3>
          <p>We couldn't find suitcases that match your criteria. Try browsing all suitcases instead.</p>
        </div>
      `;
      return;
    }

    container.innerHTML = recommendations.map(rec => `
      <div class="recommendation-item">
        <div class="recommendation-header">
          <h3>${rec.name}</h3>
          <span class="match-score">${rec.score}% Match</span>
        </div>
        <div class="recommendation-categories">
          <strong>Features:</strong>
          ${(rec.categories || []).map(cat => `<span class="category-tag">${cat}</span>`).join('')}
        </div>
        <a href="/suitcases/${rec.uuid}" class="recommendation-btn">View Suitcase</a>
      </div>
    `).join('');
  }

  function resetSubmitButton() {
    if (!submitBtn) return;
    submitBtn.disabled = false;
    submitBtn.textContent = 'See Recommendations';
  }

  // CSRF
  function getCSRFToken() {
    const name = 'csrftoken=';
    const cookies = document.cookie ? document.cookie.split(';') : [];
    for (let c of cookies) {
      c = c.trim();
      if (c.startsWith(name)) return decodeURIComponent(c.slice(name.length));
    }
    return null;
  }
});
